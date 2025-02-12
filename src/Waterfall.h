#pragma once

#include "FlowingFFTSpectre.h"
#include "../include/GLFW/glfw3.h"
#include "FastMath.h"
#include <algorithm>

class Waterfall {

	int div = 1;

	int size = 1200;

	GLuint* texturesArray = new GLuint[size];

	//int minColor = 0x1B1BB3; //blue
	//int minColor = 0x020C22;
	//int minColor = 0x0e0e0e; //interface color
	//int minColor = 0x0e0e0e;
	//int maxColor = 0xFFE800; //yellow
	//int maxColor = 0xFF0000; //red
	//int maxColor = 0x00CC00; //red

	unsigned int depth = 4;

	GLuint texName = NULL;

	ViewModel* viewModel;

	int lineHeight = 1;

	Config* config;

	FastMath fm;

public:

	Waterfall(Config* config, ViewModel* viewModel);

	float minValue = -100;
	float maxValue = -40;

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

	int oldToNew(int oldVal, int oldMin, int oldMax, int newMin, int newMax);

	void update(FFTData::OUTPUT* spectreData, FlowingSpectre* flowingSpec, SpectreHandler* specHandler);

	int getSize();

	RGB convertColor(int hexValue);

	int interpolate(int color1, int color2, float fraction);

	RGB getColorForPowerInSpectre(float power, float minValue, float maxValue);

	void setMinMaxValue(float min, float max);

	void clear();

private:
	WATERFALL_TEXTURE_STRUCT textureStruct {};
};