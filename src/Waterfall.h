#pragma once

#include "vector"
#include "stack"
#include "queue"
#include "FFTSpectreHandler.h"
#include "../include/GLFW/glfw3.h"

class Waterfall {

	float div = 1;

	int size = 200;

	GLuint* texturesArray = new GLuint[size];

	float** waterfallData = new float* [size];
	//std::vector<float*> waterfallData;
	//std::queue<float*> waterfallData;

	int waterfallWidthPx;

	int minColor = 0x1B1BB3; //blue
	int maxColor = 0xFFE800; //yellow

	float minValue = -120;
	float maxValue = -30;

	int widthPX;
	int heightPX;

	//GLubyte checkImage[heightPX][widthPX][4];

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

	void putData(FFTSpectreHandler* fftSH, float* spectreData, int len);

	float* getDataFor(int point);

	int getSize();

	RGB convertColor(int hexValue);

	int interpolate(int color1, int color2, float fraction);

	//int interpolate2(int color1, int color2, float progress, int interpolation);

	WATERFALL_TEXTURE_STRUCT generateWaterfallTexture();

	WATERFALL_TEXTURE_STRUCT getTextureStruct();

	RGB getColorForPowerInSpectre(float power);

	void setMinMaxValue(float min, float max);

	void process();

	std::thread start();

private:
	WATERFALL_TEXTURE_STRUCT textureStruct;
};