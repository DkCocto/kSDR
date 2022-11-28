#pragma once

#include "vector"
#include "stack"
#include "queue"
#include "FFTSpectreHandler.h"
#include "../include/GLFW/glfw3.h"

class Waterfall {

	float div = 1;

	int size = 500;

	GLuint* texturesArray = new GLuint[size];

	int waterfallWidthPx;

	//int minColor = 0x1B1BB3; //blue
	//int minColor = 0x020C22;
	int minColor = 0x0e0e0e;
	int maxColor = 0xFFE800; //yellow
	//int maxColor = 0xFF0000; //red
	//int maxColor = 0x00CC00; //red


	float minValue = -100;
	float maxValue = -40;

	unsigned int depth = 4;

	GLuint texName;

	FFTSpectreHandler* fftSH;

public:

	Waterfall(Config* config, FFTSpectreHandler* fftSH);

	typedef struct RGB {
		int r;
		int g;
		int b;
	};

	typedef struct WATERFALL_TEXTURE_STRUCT {
		GLuint texName;
		int width;
		int height;
	};

	GLuint* getTexturesArray();

	float getDiv();

	void putData(FFTSpectreHandler* fftSH, float* spectreData, int lineHeight);

	//float* getDataFor(int point);

	int getSize();

	RGB convertColor(int hexValue);

	int interpolate(int color1, int color2, float fraction);

	//int interpolate2(int color1, int color2, float progress, int interpolation);

	//WATERFALL_TEXTURE_STRUCT generateWaterfallTexture();

	//WATERFALL_TEXTURE_STRUCT getTextureStruct();

	RGB getColorForPowerInSpectre(float power);

	void setMinMaxValue(float min, float max);


private:
	WATERFALL_TEXTURE_STRUCT textureStruct;
};