#include "Waterfall.h"

Waterfall::Waterfall(Config* config, FFTSpectreHandler* fftSH) {
	waterfallWidthPx = (config->fftLen / 2) / div;
	this->fftSH = fftSH;
}

float Waterfall::getDiv() {
	return div;
}

void Waterfall::putData(FFTSpectreHandler* fftSH, float* spectreData, int lineHeight) {
	float sum = 0;

	float* output = new float[fftSH->getSpectreSize() / div];

	for (int i = 1; i <= fftSH->getSpectreSize(); i++) {

		sum += spectreData[fftSH->getTrueBin(i - 1)];

		if (i % (int)div == 0) {
			output[i / (int)div - 1] = sum / div;
			sum = 0;
		}
	}

	int height = lineHeight;
	int width = waterfallWidthPx;

	GLubyte* checkImage = new GLubyte[width * height * depth];

	for (unsigned int ix = 0; ix < height; ++ix) {
		for (unsigned int iy = 0; iy < width; ++iy) {
			Waterfall::RGB rgb = getColorForPowerInSpectre(output[iy]);

			checkImage[ix * width * depth + iy * depth + 0] = rgb.r;   //red
			checkImage[ix * width * depth + iy * depth + 1] = rgb.g;   //green
			checkImage[ix * width * depth + iy * depth + 2] = rgb.b;   //blue
			checkImage[ix * width * depth + iy * depth + 3] = 255; //alpha
		}
	}

	delete[] output;

	GLuint texName;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);

	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);

	delete[] checkImage;

	glDeleteTextures(1, &texturesArray[size - 1]);

	for (int i = 0; i < size - 1; i++) {
		texturesArray[(size - 1) - i] = texturesArray[(size - 1) - i - 1];
	}

	texturesArray[0] = texName;
}

int Waterfall::getSize() {
	return size;
}

Waterfall::RGB Waterfall::convertColor(int hexValue) {
	Waterfall::RGB rgbColor;

	rgbColor.r = hexValue / 0x10000;			// Extract the RR byte
	rgbColor.g = (hexValue / 0x100) % 0x100;    // Extract the GG byte
	rgbColor.b = hexValue % 0x100;				// Extract the BB byte

	return rgbColor;
}

/**
 * interpolate 2 RGB colors
 * @param color1    integer containing color as 0x00RRGGBB
 * @param color2    integer containing color as 0x00RRGGBB
 * @param fraction  how much interpolation (0..1)
 * - 0: full color 1
 * - 1: full color 2
 * @return the new color after interpolation
 */
int Waterfall::interpolate(int color1, int color2, float fraction) {

	if (fraction < 0) fraction = 0;
	if (fraction > 1) fraction = 1;

	unsigned char   r1 = (color1 >> 16) & 0xff;
	unsigned char   r2 = (color2 >> 16) & 0xff;
	unsigned char   g1 = (color1 >> 8) & 0xff;
	unsigned char   g2 = (color2 >> 8) & 0xff;
	unsigned char   b1 = color1 & 0xff;
	unsigned char   b2 = color2 & 0xff;

	float f = (1.0 - cos(fraction * M_PI)) * 0.5f;

	//float f = fraction;

	//float f = sqrt(fraction)

	return (int)((r2 - r1) * f + r1) << 16 |
		(int)((g2 - g1) * f + g1) << 8 |
		(int)((b2 - b1) * f + b1);
}

Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power) {
	float fraction = (1.0 / (maxValue - minValue)) * power - (minValue / (maxValue - minValue));
	int interpolatedColor = interpolate(minColor, maxColor, fraction);
	RGB rgb = convertColor(interpolatedColor);
	return rgb;
}

void Waterfall::setMinMaxValue(float min, float max) {
	minValue = min;
	maxValue = max;
}

GLuint* Waterfall::getTexturesArray() {
	return texturesArray;
}