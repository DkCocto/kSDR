#include "WindowBlackman.h"

WindowBlackman::WindowBlackman(int fftLen) {
    this->len = fftLen / 2;
    init();
}

WindowBlackman::~WindowBlackman() {
    delete[] weightArray;
}

float* WindowBlackman::getWeights() {
    return weightArray;
}

void WindowBlackman::init() {
    weightArray = new float[len];
    for (int i = 0; i < len; i++) {
        weightArray[i] = (0.42 - 0.5 * cosf(2.0 * M_PI * i / len) + 0.08 * cosf(4.0 * M_PI * i / len));
        if (weightArray[i] < 0.0) weightArray[i] = 0.0;
    }
}