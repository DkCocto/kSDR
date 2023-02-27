#pragma once

#include "windows.h"
#include "KalmanFilter.h"

class CPU {

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE self;

    KalmanFilter* kalman = new KalmanFilter(1, 0.01);

public:
    CPU();
    void init();
    double getCurrentValue();
};