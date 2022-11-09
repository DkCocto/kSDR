#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(double standardDeviation, double speed) {
    this->standardDeviation = standardDeviation;
    this->speed = speed;
}

double KalmanFilter::filter(double value) {
    if (-1000.0 < value < 1000.0) {
        pc = p + speed;
        g = pc / (pc + standardDeviation);
        p = (1.0 - g) * pc;
        xp = xe;
        zp = xp;
        xe = g * (value - zp) + xp;
        return xe;
    } else {
        return value;
    }
}