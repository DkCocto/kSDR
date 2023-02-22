#pragma once

#include "Display.h"
#include "ReceiverLogic.h"
#include "Average.h"
#include "cstring"
#include "cmath"
#include "vector"
#include "KalmanFilter.h"

class AGC {

private:
    double threshold = 0.005;

    double amp = 1;

    Average* averageAmp = new Average(15);

    int count = 0;

    double signalAbsAverage = 0.0;

    FFTSpectreHandler* fftSpectreHandler;

    float max = 0;

    double savedValue = 0;
    double delta = 0;

    Config* config;

public:

    AGC(Config* config, FFTSpectreHandler* fftSpectreHandler);

    double process(double signal);

    double getAmp();

};