#include "Delay.h"

Delay::Delay(int len) {
    this->len = len;
    xv = new double[len + 1];
}

double Delay::filter(double value) {
    for (int i = 0; i < len; i++) xv[i] = xv[i + 1];
    xv[len] = value;
    return xv[0];
}