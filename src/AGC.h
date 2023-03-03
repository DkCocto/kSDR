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
    double threshold = 0.05;

    double amp = 1;

    Average* averageAmp = new Average(15);
    Average* avg = new Average(150);

    int count = 0;

    double signalAbsAverage = 0.0;

    FFTSpectreHandler* fftSpectreHandler;

    float max = 0;

    double savedValue = 0;
    double delta = 0;

    Config* config;

public:

    AGC(Config* config, FFTSpectreHandler* fftSpectreHandler);

    //double process(double signal);

    double processNew(double signal);

    void atack(double signal, double speed);

    void releaseAtack(double signal, double speed);

    double getAmp();

};