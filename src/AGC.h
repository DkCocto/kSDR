#pragma once

#include "math.h"
#include "FFTSpectreHandler.h"

class AGC {

private:
    float threshold = 0.05;

    float amp = 1;

    int count = 0;

    float signalAbsAverage = 0.0;

    SpectreHandler* specHandler = nullptr;

    float max = 0;

    float savedValue = 0;
    float delta = 0;

    Config* config = nullptr;

public:

    AGC() {};

    AGC(Config* config, SpectreHandler* specHandler);

    float processNew(float signal);

    void atack(float signal, float speed);

    void releaseAtack(float signal, float speed);

    float getAmp();

};