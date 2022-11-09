#pragma once
class Delay {

    int len = 0;
    double* xv;

public:

    Delay(int len);

    double filter(double value);
};