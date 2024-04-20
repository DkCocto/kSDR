#pragma once

#include "../include/GLFW/glfw3.h"
#include "Waterfall.h"

class ColoredSpectreBG {

	int depth = 4;

	GLuint image;

public:

	struct params {
		int width;
		int height;
		float signalMinDb;
		float signalMaxDb;
		float minDb;
		float maxDb;
	};

	void generateImage(Waterfall* waterfall, params p);
	GLuint getImage();

private:

	params savedParams;
};