#pragma once

#include "stdio.h"

class KalmanFilter {
private:

    double standardDeviation, speed;

    double pc = 0.0;
    double g = 0.0;
    double p = 1.0;
    double xp = 0.0;
    double zp = 0.0;
    double xe = 0.0;

public:
    KalmanFilter() {};
    KalmanFilter(double standardDeviation, double speed);
    double filter(double value);
};