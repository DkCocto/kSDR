#pragma once
#include "Env.h"

class WindowBlackman{
    int len = 0;
    float* weightArray;
public:

    WindowBlackman(int fftLen) {
        this->len = fftLen / 2;
        init();
    }
    ~WindowBlackman() {
        delete[] weightArray;
    }

    float* getWeights() {
        return weightArray;
    }

    void init() {
        weightArray = new float[len];
        for (int i = 0; i < len; i++) {
            weightArray[i] = (0.42 - 0.5 * cosf(2.0 * M_PI * i / len) + 0.08 * cosf(4.0 * M_PI * i / len));
            if (weightArray[i] < 0.0) weightArray[i] = 0.0;
        }
    }
};