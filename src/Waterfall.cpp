#include "Waterfall.h"

Waterfall::Waterfall(Config* config, ViewModel* viewModel) {
	memset(texturesArray, 0, sizeof(texturesArray) * size);
	this->viewModel = viewModel;
	this->config = config;
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
	std::vector<float> waterfallData = flowingSpec->getReducedData(spectreData, config->visibleSpectreBinCount, specHandler);

	int width = waterfallData.size() / div;
	int height = lineHeight;

	std::vector<float> output(width, 0.0f);
	float sum = 0.0f;
	int count = 0;

	for (size_t i = 0; i < waterfallData.size(); ++i) {
		sum += waterfallData[i];
		count++;

		if (count == div) {
			output[i / div] = sum / div;
			sum = 0.0f;
			count = 0;
		}
	}

	// Создаем новую строку данных для водопада
	GLubyte* newLine = new GLubyte[width * depth];

	for (int iy = 0; iy < width; ++iy) {
		Waterfall::RGB rgb = getColorForPowerInSpectre(output[iy], minValue, maxValue);
		newLine[iy * depth + 0] = rgb.r;
		newLine[iy * depth + 1] = rgb.g;
		newLine[iy * depth + 2] = rgb.b;
		newLine[iy * depth + 3] = 255;
	}

	// Удаляем старую текстуру (самую нижнюю)
	glDeleteTextures(1, &texturesArray[size - 1]);

	// Сдвигаем массив текстур вниз
	for (int i = size - 1; i > 0; --i) {
		texturesArray[i] = texturesArray[i - 1];
	}

	// Генерируем новую текстуру (для верхней строки)
	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newLine);

	// Записываем новую текстуру в массив
	texturesArray[0] = texName;

	delete[] newLine;
}



/*void Waterfall::update(FFTData::OUTPUT* spectreData, FlowingSpectre* flowingSpec, SpectreHandler* specHandler) {

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
			Waterfall::RGB rgb = getColorForPowerInSpectre(output[iy], minValue, maxValue + 5);

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
}*/

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

/*Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power, float minValue, float maxValue) {
	float fraction = (power - minValue) / (maxValue - minValue);
	fraction = std::clamp(fraction, 0.0f, 1.0f); // Ограничиваем 0..1

	// Логарифмическое усиление слабых сигналов
	float gamma = 0.95f;  // Чем меньше, тем лучше видны слабые сигналы
	fraction = std::pow(fraction, gamma);

	// Цветовая карта SDR Console
	const int numColors = 8;
	const Waterfall::RGB colormap[numColors] = {
		{  0,   0, 128},  // Темно-синий
		{  0,   0, 255},  // Синий
		{  0, 255, 255},  // Голубой
		{  0, 255,   0},  // Зеленый
		{255, 255,   0},  // Желтый
		{255, 128,   0},  // Оранжевый
		{255,   0,   0},  // Красный
		{128,   0,   0}   // Темно-красный
	};

	// Определяем индекс в градиенте
	float scaledIndex = fraction * (numColors - 1);
	int idx1 = static_cast<int>(scaledIndex);
	int idx2 = (idx1 + 1 < numColors) ? idx1 + 1 : idx1;
	float t = scaledIndex - idx1;  // Доля между цветами

	// Применяем сглаживание (smoothstep)
	t = t * t * t * (t * (t * 6 - 15) + 10);

	// Интерполяция (LERP) с плавным переходом
	int r = static_cast<int>((1 - t) * colormap[idx1].r + t * colormap[idx2].r);
	int g = static_cast<int>((1 - t) * colormap[idx1].g + t * colormap[idx2].g);
	int b = static_cast<int>((1 - t) * colormap[idx1].b + t * colormap[idx2].b);

	return { std::clamp(r, 0, 255), std::clamp(g, 0, 255), std::clamp(b, 0, 255) };
}*/

// Цвета градиента (можно настраивать)
struct ColorPoint {
	float r, g, b;
	float position; // Позиция в градиенте (0..1)
};



/*std::vector<ColorPoint> gradient = {
	{0.0f, 0.0f, 0.5f, 0.0f},    // Темно-синий
	{0.0f, 0.0f, 1.0f, 0.125f},  // Синий
	{0.0f, 1.0f, 1.0f, 0.175f},  // Голубой
	{0.0f, 1.0f, 0.0f, 0.3f},    // Зеленый
	{1.0f, 1.0f, 0.0f, 0.8f},   // Желтый
	{1.0f, 0.5f, 0.0f, 1.0f}    // Оранжевый
};*/
/*Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power, float minValue, float maxValue) {
	float fraction = (power - minValue) / (maxValue - minValue);
	fraction = std::clamp(fraction, 0.0f, 1.0f); // Ограничиваем 0..1

	// Гамма-коррекция для повышения насыщенности
	float gamma = 0.95f;
	fraction = std::pow(fraction, gamma);
	
	std::vector<ColorPoint> gradient = {
	   {0.0f, 0.0f, 0.0f, 0.0f},    // Черный (слабые сигналы)
	   {0.0f, 0.0f, 1.0f, 0.f},    // Синий
	   {0.0f, 1.0f, 0.0f, 0.35f},    // Зеленый
	   {1.0f, 1.0f, 0.0f, 0.55f},    // Желтый
	   {1.0f, 0.5f, 0.0f, 0.85f},    // Оранжевый
	   {1.0f, 0.0f, 0.0f, 1.0f},    // Красный
	};

	// Находим два ближайших цвета для интерполяции
	ColorPoint* start = &gradient[0];
	ColorPoint* end = &gradient[1];
	for (size_t i = 1; i < gradient.size(); ++i) {
		if (fraction <= gradient[i].position) {
			start = &gradient[i - 1];
			end = &gradient[i];
			break;
		}
	}

	// Интерполяция между двумя цветами
	float t = (fraction - start->position) / (end->position - start->position);

	// Плавная интерполяция (выберите один из методов)
	auto interpolate = [](float start, float end, float t) {
		// Линейная интерполяция
		// return start + (end - start) * t;

		// Косинусная интерполяция
		//float smoothT = (1.0f - std::cos(t * M_PI)) * 0.5f;
		// return start + (end - start) * smoothT;

		// Интерполяция Smoothstep
		float smoothT = t * t * (3.0f - 2.0f * t);
		return start + (end - start) * smoothT;
	};

	float r = interpolate(start->r, end->r, t);
	float g = interpolate(start->g, end->g, t);
	float b = interpolate(start->b, end->b, t);

	return {
		std::clamp(static_cast<int>(255.0 * r), 0, 255),
		std::clamp(static_cast<int>(255.0 * g), 0, 255),
		std::clamp(static_cast<int>(255.0 * b), 0, 255)
	};
}*/


Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power, float minValue, float maxValue) {
	float fraction = (power - minValue) / (maxValue - minValue);
	fraction = std::clamp(fraction, 0.0f, 1.0f); // Ограничиваем 0..1

	// Гамма-коррекция для повышения насыщенности
	float gamma = 1.05;
	fraction = std::pow(fraction, gamma);
	//fraction = (1.0 - cos(fraction * M_PI)) * 0.75;

	float r, g, b;
	if (fraction < 0.125f) { r = 0.0f; g = 0.0f; b = 4.0f * fraction + 0.5f; }
	else if (fraction < 0.375f) { r = 0.0f; g = 4.0f * (fraction - 0.125f); b = 1.0f; }
	else if (fraction < 0.800f) { r = 4.0f * (fraction - 0.375f); g = 1.0f; b = 1.0f - 4.0f * (fraction - 0.375f); }
	else if (fraction < 0.99999f) { r = 1.0f; g = 1.0f - 4.0f * (fraction - 0.625f); b = 0.0f; }
	else { r = 1.0f - 0.5f * (fraction - 0.875f); g = 0.0f; b = 0.0f; }

	return {
		std::clamp(static_cast<int>(255.0 * r), 0, 255),
		std::clamp(static_cast<int>(255.0 * g), 0, 255),
		std::clamp(static_cast<int>(255.0 * b), 0, 255)
	};
}

/*Waterfall::RGB Waterfall::getColorForPowerInSpectre(float power, float minValue, float maxValue) {
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
}*/

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