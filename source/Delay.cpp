#include "Delay.h"

Delay::Delay(int len) {
    this->len = len;
    xv = new float[len + 1];
}

Delay::~Delay() {
    delete[] xv;
}

float Delay::filter(float value) {
    for (int i = 0; i < len; i++) xv[i] = xv[i + 1];
    xv[len] = value;
    return xv[0];
}