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
    double threshold = 0.01;

    double amp = 1;

    Average* averageAmp = new Average(15);

    int count = 0;

    //int bufLen = 512;
    //double* buf = new double[bufLen];

    //std::vector<double> buf;

    double signalAbsAverage = 0.0;

    FFTSpectreHandler* fftSpectreHandler;

    float max = 0;

    double savedValue = 0;
    double delta = 0;

    Config* config;

public:

    AGC(Config* config, FFTSpectreHandler* fftSpectreHandler);

    //double processNew(double signal);

    double process(double signal);

    double getAmp();

    //double canculateAmplitudeAverage();
};