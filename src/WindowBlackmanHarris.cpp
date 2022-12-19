#include "WindowBlackmanHarris.h"

WindowBlackmanHarris::WindowBlackmanHarris(int fftLen) {
	this->len = fftLen / 2;
	init();
}

float* WindowBlackmanHarris::getWeights() {
	return weightArray;
}

void WindowBlackmanHarris::init() {
    const float a0 = 0.35875f;
    const float a1 = 0.48829f;
    const float a2 = 0.14128f;
    const float a3 = 0.01168f;

    unsigned int idx = 0;
    weightArray = new float[len];
    while (idx < len) {
        weightArray[idx] = a0 - (a1 * cosf((2.0f * M_PI * idx) / (len - 1))) + (a2 * cosf((4.0f * M_PI * idx) / (len - 1))) - (a3 * cosf((6.0f * M_PI * idx) / (len - 1)));
        idx++;
    }
}
