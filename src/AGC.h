#pragma once

#include "math.h"
#include "FFTSpectreHandler.h"

class AGC {

private:
    float threshold = 0.05f;

    float amp = 1.0f;

    int count = 0;

    float signalAbsAverage = 0.0f;

    SpectreHandler* specHandler = nullptr;

    float max = 0.0f;

    float savedValue = 0.0f;
    float delta = 0.0f;

    Config* config = nullptr;

public:

    AGC() {};

    AGC(Config* config, SpectreHandler* specHandler);

    float processNew(float signal);

    void atack(float signal, float speed);

    void releaseAtack(float signal, float speed);

    float getAmp();

};