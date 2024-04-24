#pragma once

#include <vector>

#include "cmath"

class Filter {
    /**
     * Calculate the values for a Raised Cosine filter
     * @param sampleRate
     * @param freq
     * @param len
     */
public:
    static std::vector<double> makeRaiseCosine(double sampleRate, double freq, double alpha, int len);
};