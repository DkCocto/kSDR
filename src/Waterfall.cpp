#include "Waterfall.h"

unsigned char newRange[256]{};

Waterfall::Waterfall(Config* config, ViewModel* viewModel) {
	memset(texturesArray, 0, sizeof(texturesArray) * size);
	this->viewModel = viewModel;
	this->config = config;

	for (size_t i{}; i < 256; ++i) {
		newRange[i] = oldToNew(i, 25, 230, 25, 230);
	}
}

float Waterfall::getDiv() {
	return div;
}

int Waterfall::oldToNew(int oldVal, int oldMin, int oldMax, int newMin, int newMax) {
	int retval =
		((oldVal - oldMin) * (newMax - newMin) / (oldMax - oldMin)) + newMin;
	retval = retval < 0 ? 0 : retval;
	retval = retval > 255 ? 255 : retval;
	return retval;
}

void Waterfall::update(FFTData::OUTPUT* spectreData, FlowingSpectre* flowingSpec, SpectreHandler* specHandler) {

	//std::vector<float> fullSpectreData = flowingFFTSpectre->getSpectreHandler()->getFFTData()->getData(true);
	
	std::vector<float> waterfallData = flowingSpec->getReducedData(spectreData, config->visibleSpectreBinCount, specHandler);

	//delete[] fullSpectreData;

	float sum = 0.0f;

	float* output = new float[waterfallData.size() / div];

	int start = 1;							 //1
	int stop = waterfallData.size();	 //flowingFFTSpectre->getLen()

	//printf("%d %d %d \r\n", flowingFFTSpectre->getLen(), flowingFFTSpectre->getA(), flowingFFTSpectre->getB());

	for (int i = start; i <= stop; i++) {

		sum += waterfallData[i - 1];

		if (i % div == 0) {
			output[i / div - 1] = sum / (float)div;
			sum = 0;
		}
	}

	int height = lineHeight;
	int width = waterfallData.size() / div;

	GLubyte* checkImage = new GLubyte[width * height * depth];

	for (int ix = 0; ix < height; ++ix) {
		for (int iy = 0; iy < width; ++iy) {
			//6 10
			Waterfall::RGB rgb = getColorForPowerInSpectre(output[iy], minValue - 6, maxValue + 10);

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

	//float f = (1.0 - cos(fraction * M_PI)) * 0.5f;

	float f = fraction * fraction;

	//float f = sqrt(fraction);

	return (int)((r2 - r1) * f + r1) << 16 |
		(int)((g2 - g1) * f + g1) << 8 |
		(int)((b2 - b1) * f + b1);
}

Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power, float minValue, float maxValue) {
	float fraction = (1.0 / (maxValue - minValue)) * power - (minValue / (maxValue - minValue));

	//myFastCos
	float f = (1.0 - fm.myFastCos(fraction * M_PI)) * 0.5;
	//float f = fraction * fraction;
	//float f = fraction;
	//int interpolatedColor = interpolate(minColor, maxColor, fraction);
	//RGB rgb = convertColor(interpolatedColor);

	//RgbCalculator rgbCalc;
	//RgbCalculator::RGB rgb_ = rgbCalc.Calc((power - (-100)) / (0 - (-100)) * 1000);


	double t;  
	double r = 0.0; 
	double g = 0.0; 
	double b = 0.0;
	
	//680
	//380

	double l = (f - (0)) / (1.0 - 0) * (680.0 - 400) + 400;

	if ((l >= 400.0) && (l < 410.0)) { t = (l - 400.0) / (410.0 - 400.0); r = +(0.33 * t) - (0.20 * t * t); }
	else if ((l >= 410.0) && (l < 475.0)) { t = (l - 410.0) / (475.0 - 410.0); r = 0.14 - (0.13 * t * t); }
	else if ((l >= 545.0) && (l < 595.0)) { t = (l - 545.0) / (595.0 - 545.0); r = +(1.98 * t) - (t * t); }
	else if ((l >= 595.0) && (l < 650.0)) { t = (l - 595.0) / (650.0 - 595.0); r = 0.98 + (0.06 * t) - (0.40 * t * t); }
	else if ((l >= 650.0) && (l < 700.0)) { t = (l - 650.0) / (700.0 - 650.0); r = 0.65 - (0.84 * t) + (0.20 * t * t); }
	if ((l >= 415.0) && (l < 475.0)) { t = (l - 415.0) / (475.0 - 415.0); g = +(0.80 * t * t); }
	else if ((l >= 475.0) && (l < 590.0)) { t = (l - 475.0) / (590.0 - 475.0); g = 0.8 + (0.76 * t) - (0.80 * t * t); }
	else if ((l >= 585.0) && (l < 639.0)) { t = (l - 585.0) / (639.0 - 585.0); g = 0.84 - (0.84 * t); }
	if ((l >= 400.0) && (l < 475.0)) { t = (l - 400.0) / (475.0 - 400.0); b = +(2.20 * t) - (1.50 * t * t); }
	else if ((l >= 475.0) && (l < 560.0)) { t = (l - 475.0) / (560.0 - 475.0); b = 0.7 - (t)+(0.30 * t * t); }


	RGB rgb{ (int)(255.0 * r), (int)(255.0 * g), (int)(255.0 * b) };

	return rgb;
}

void Waterfall::setMinMaxValue(float min, float max) {
	minValue = min;
	maxValue = max;
}

void Waterfall::clear() {
	memset(texturesArray, 0, sizeof(texturesArray) * size);
}

GLuint* Waterfall::getTexturesArray() {
	return texturesArray;
}