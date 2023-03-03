#include "ColoredSpectreBG.h"

void ColoredSpectreBG::generateImage(Waterfall* waterfall, params p) {
	if (savedParams.height == p.height && 
		savedParams.width == p.width &&
		savedParams.minDb == p.minDb &&
		savedParams.maxDb == p.maxDb &&
		savedParams.signalMaxDb == p.signalMaxDb &&
		savedParams.signalMinDb == p.signalMinDb) return;

	GLubyte* imageData = new GLubyte[p.width * p.height * depth];

	for (unsigned int ix = 0; ix < p.height; ++ix) {
		for (unsigned int iy = 0; iy < p.width; ++iy) {

			// [0, height] -> [minDb, maxDb]
			//return (value - From1) / (From2 - From1) * (To2 - To1) + To1;
			float val = ((p.height - ix)  - 0.0) / (p.height - 0.0) * (p.signalMaxDb - p.signalMinDb) + p.signalMinDb;

			Waterfall::RGB rgb = waterfall->getColorForPowerInSpectre(val, p.minDb, p.maxDb);

			imageData[ix * p.width * depth + iy * depth + 0] = rgb.r;   //red
			imageData[ix * p.width * depth + iy * depth + 1] = rgb.g;   //green
			imageData[ix * p.width * depth + iy * depth + 2] = rgb.b;   //blue
			imageData[ix * p.width * depth + iy * depth + 3] = 255;		//alpha
		}
	}

	glDeleteTextures(1, &image);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &image);

	glBindTexture(GL_TEXTURE_2D, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p.width,
		p.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		imageData);

	delete[] imageData;

	savedParams = p;
}

GLuint ColoredSpectreBG::getImage() {
	return image;
}
