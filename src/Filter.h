#pragma once

#include "Env.h"
#include "cmath"

class Filter {
    /**
     * Calculate the values for a Raised Cosine filter
     * @param sampleRate
     * @param freq
     * @param len
     */
public:
    static double* makeRaiseCosine(double sampleRate, double freq, double alpha, int len);
};