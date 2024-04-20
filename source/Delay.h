#pragma once
class Delay {

    int len = 0;
    float* xv = nullptr;

public:
    Delay(int len);

    ~Delay();

    float filter(float value);
};