#pragma once

class KalmanFilter {
private:

    float standardDeviation, speed;

    float pc = 0.0f;
    float g = 0.0f;
    float p = 1.0f;
    float xp = 0.0f;
    float zp = 0.0f;
    float xe = 0.0f;

public:
    KalmanFilter() {};
    KalmanFilter(float standardDeviation, float speed);
    float filter(float value);
};