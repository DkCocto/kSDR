#pragma once

#include "windows.h"
#include <memory>
#include "Average.h"


class CPU {

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE self;

    std::unique_ptr<Average> average;

public:
    CPU();
    void init();
    double getCurrentValue();
};