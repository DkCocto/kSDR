#include "Waterfall.h"

Waterfall::Waterfall(Config* config, FFTSpectreHandler* fftSH) {
	waterfallWidthPx = (config->fftLen / 2) / div;
	this->fftSH = fftSH;

	for (int i = 0; i < size; i++) {
		waterfallData[i] = new float[config->fftLen / div];
	}
}

float Waterfall::getDiv() {
	return div;
}

void Waterfall::putData(FFTSpectreHandler* fftSH, float* spectreData, int len) {
	float sum = 0;

	float* output = new float[len / div];

	//std::vector<float> out;

	for (int i = 1; i <= len; i++) {

		sum += spectreData[fftSH->getTrueBin(i - 1)];

		//Utils::printFloat(spectreData[fftSH->getTrueBin(i - 1)]);

		if (i % (int)div == 0) {
			output[i / (int)div - 1] = sum / div;
			//Utils::printFloat(sum / div);
			//out.push_back(sum / div);
			sum = 0;
		}
	}

	/*for (int i = 0; i < size - 1; i++) {
		waterfallData[(size - 1) - i] = waterfallData[(size - 1) - i - 1];
	}
	waterfallData[0] = output;*/


	int height = 1;
	int width = waterfallWidthPx;

	GLubyte* checkImage = new GLubyte[width * height * depth];

	for (unsigned int ix = 0; ix < height; ++ix) {
		for (unsigned int iy = 0; iy < width; ++iy) {
			//int c = (((ix & 0x8) == 0) ^ ((iy & 0x8)) == 0) * 255;

			//float* currentSpectre = getDataFor((getSize() - 1) - ix);

			Waterfall::RGB rgb = getColorForPowerInSpectre(output[iy]);

			//printf("%i\r\n", rgb.r);

			checkImage[ix * width * depth + iy * depth + 0] = rgb.r;   //red
			checkImage[ix * width * depth + iy * depth + 1] = rgb.g;   //green
			checkImage[ix * width * depth + iy * depth + 2] = rgb.b;   //blue
			checkImage[ix * width * depth + iy * depth + 3] = 255; //alpha
		}
	}

	delete[] output;

	//glDeleteTextures(0, &texName);

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

	for (int i = 0; i < size - 1; i++) {
		texturesArray[(size - 1) - i] = texturesArray[(size - 1) - i - 1];
	}

	glDeleteTextures(0, &texturesArray[0]);

	texturesArray[0] = texName;


	/*
	if (waterfallData.size() > size) {
		//std::rotate(waterfallData.begin(), waterfallData.begin() + 1, waterfallData.end());
		//waterfallData.erase(waterfallData.end());
		//float* array = waterfallData.back();
		//waterfallData.pop_back();
		waterfallData.pop();
		//delete[] array;
	}

	waterfallData.push(output);*/
}

float* Waterfall::getDataFor(int point) {
	return waterfallData[point];
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
	unsigned char   r1 = (color1 >> 16) & 0xff;
	unsigned char   r2 = (color2 >> 16) & 0xff;
	unsigned char   g1 = (color1 >> 8) & 0xff;
	unsigned char   g2 = (color2 >> 8) & 0xff;
	unsigned char   b1 = color1 & 0xff;
	unsigned char   b2 = color2 & 0xff;

	return (int)((r2 - r1) * fraction + r1) << 16 |
		(int)((g2 - g1) * fraction + g1) << 8 |
		(int)((b2 - b1) * fraction + b1);
}

/*int Waterfall::interpolate2(int color1, int color2, float progress, int interpolation) {
	//Разделяем оба цвета на составляющие
	int a1 = (color1 & 0xff000000) >>> 24;
	int r1 = (color1 & 0x00ff0000) >>> 16;
	int g1 = (color1 & 0x0000ff00) >>> 8;
	int b1 = color1 & 0x000000ff;

	int a2 = (color2 & 0xff000000) >> > 24;
	int r2 = (color2 & 0x00ff0000) >> > 16;
	int g2 = (color2 & 0x0000ff00) >> > 8;
	int b2 = color2 & 0x000000ff;

	//И рассчитываем новые
	float f;
	if (interpolation == INTERPOLATION_LINEAR) {
		f = progress;
	}
	else if (interpolation == INTERPOLATION_COS) {
		float ft = progress * M_PI;
		f = (1 - (float)cos(ft)) * 0.5f;
	}
	else {
		throw new IllegalArgumentException();
	}
	int newA = clip((int)(a1 * (1 - f) + a2 * f));
	int newR = clip((int)(r1 * (1 - f) + r2 * f));
	int newG = clip((int)(g1 * (1 - f) + g2 * f));
	int newB = clip((int)(b1 * (1 - f) + b2 * f));

	//Собираем и возвращаем полученный цвет
	return (newA << 24) + (newR << 16) + (newG << 8) + newB;
}*/

Waterfall::WATERFALL_TEXTURE_STRUCT Waterfall::generateWaterfallTexture() {

	int height = size;
	int width = waterfallWidthPx;

	GLubyte* checkImage = new GLubyte[height * width * depth];

	for (unsigned int ix = 0; ix < height; ++ix) {
		for (unsigned int iy = 0; iy < width; ++iy) {
			//int c = (((ix & 0x8) == 0) ^ ((iy & 0x8)) == 0) * 255;

			float* currentSpectre = getDataFor((getSize() - 1) - ix);

			Waterfall::RGB rgb = getColorForPowerInSpectre(currentSpectre[iy]);

			//printf("%i\r\n", rgb.r);

			checkImage[ix * width * depth + iy * depth + 0] = rgb.r;   //red
			checkImage[ix * width * depth + iy * depth + 1] = rgb.g;   //green
			checkImage[ix * width * depth + iy * depth + 2] = rgb.b;   //blue
			checkImage[ix * width * depth + iy * depth + 3] = 255; //alpha
		}
	}

	glDeleteTextures(0, &texName);

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

	/*for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}*/
	delete[] checkImage;

	return WATERFALL_TEXTURE_STRUCT {texName, width, height};
	//return WATERFALL_TEXTURE_STRUCT{};
}

Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power) {
	//float fraction = 0.0077 * power + 1.0;
	float fraction = (1.0 / (maxValue - minValue)) * power - (minValue / (maxValue - minValue));
	//float fraction = exp(2.8004 + 0.0759 * power);
	//float ft = power * M_PI;
	//float f = (1 - (float)cos(ft)) * 0.5f;
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

Waterfall::WATERFALL_TEXTURE_STRUCT Waterfall::getTextureStruct() {
	return textureStruct;
}

void Waterfall::process() {
	/*while (true) {
		float* spectreData = fftSH->getOutput();
		int spectreSize = fftSH->getSpectreSize();
		
		putData(fftSH, spectreData, spectreSize);

		textureStruct = generateWaterfallTexture();

		//std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}*/
}

std::thread Waterfall::start() {
	std::thread p(&Waterfall::process, this);
	return p;
}