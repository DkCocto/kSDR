#pragma once

#include "Display.h"
#include "ReceiverLogic.h"

class AGC {

private:
    double threshold = 0.05;

    double amp = 1;

    int count = 0;

    double signalAbsAverage = 0.0;

    FFTSpectreHandler* fftSpectreHandler;

    float max = 0;

    double savedValue = 0;
    double delta = 0;

    Config* config;

public:

    AGC(Config* config, FFTSpectreHandler* fftSpectreHandler);

    double processNew(double signal);

    void atack(double signal, double speed);

    void releaseAtack(double signal, double speed);

    double getAmp();

};