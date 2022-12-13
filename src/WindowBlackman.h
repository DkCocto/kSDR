#pragma once
#include "Env.h"

class WindowBlackman {
    public:

    int len;

    float* bW;

    WindowBlackman(int fftLen) {
        this->len = fftLen * 2;
        bW = new float[len];
    }

    float* init() {
        for (int i = 0; i < len; i++) {
            bW[i] = (0.42 - 0.5 * cosf(2.0 * M_PI * i / len) + 0.08 * cosf(4.0 * M_PI * i / len));
            if (bW[i] < 0.0) bW[i] = 0.0;
        }
        return bW;
    }
};