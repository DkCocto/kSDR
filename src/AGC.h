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
    double threshold = 0.03;

    double amp = 1;

    Average* averageAmp = new Average(30);

    int count = 0;

    int bufLen = 512;
   // double* buf = new double[bufLen];

    std::vector<double> buf;

    double signalAbsAverage = 0.0;

    FFTSpectreHandler* fftSpectreHandler;

    float max = 0;

    double savedValue = 0;
    double delta = 0;

public:

    AGC(FFTSpectreHandler* fftSpectreHandler);

    double process(double signal);

    double getAmp();

    double canculateAmplitudeAverage();
};