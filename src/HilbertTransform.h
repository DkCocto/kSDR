#pragma once

#include "Env.h"
#include "cmath"
#include "cstring"

class HilbertTransform {

    double* xv; // This array holds the delayed values
    double* coeffs;

	int samplingRate = 0;
	int len = 0;
   
    double gain = 1.0;

    int M = 0;

public:
    HilbertTransform(int samplingRate, int len);

    void init();

    double filter(double val);
};