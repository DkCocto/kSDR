#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(float standardDeviation, float speed) {
    this->standardDeviation = standardDeviation;
    this->speed = speed;
}

float KalmanFilter::filter(float value) {
    if (-1000.0f < value < 1000.0f) {
        pc = p + speed;
        g = pc / (pc + standardDeviation);
        p = (1.0f - g) * pc;
        xp = xe;
        zp = xp;
        xe = g * (value - zp) + xp;
        return xe;
    } else {
        return value;
    }
}