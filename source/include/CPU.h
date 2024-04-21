#pragma once

#if defined(__linux__)
#else /* Windows*/
	#include "windows.h"
#endif

#include <memory>
#include "Average.h"


class CPU {

    unsigned long long lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;

#if defined(__linux__)
#else /* Windows*/
    HANDLE self;
#endif
    
    std::unique_ptr<Average> average;

public:
    CPU();
    void init();
    double getCurrentValue();
};