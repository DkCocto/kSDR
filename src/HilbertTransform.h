#pragma once

#include "Env.h"
#include "cmath"
#include "cstring"

class HilbertTransform {

    float* xv = nullptr; // This array holds the delayed values
    float* coeffs = nullptr;

    int samplingRate = 0;
    int len = 0;

    float gain = 1.0f;

    int M = 0;

public:

    HilbertTransform(int samplingRate, int len);

    ~HilbertTransform();

    void init();

    float filter(float val);
};