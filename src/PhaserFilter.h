#pragma once

#include <vector>
#include <cmath>
#include "Env.h"

class PhaserFilter {
public:
    PhaserFilter(){}
    PhaserFilter(float sampleRate, float freq = 320.0f, int stages = 16)
        : stages(stages), apfBuffers(stages, 0.0f), apfCoeffs(stages, 0.0f) {

        // Рассчитываем коэффициенты APF на основе заданной частоты
        float theta = 2.0f * M_PI * freq / sampleRate;
        float sinTheta = std::sin(theta);
        float coeff = (sinTheta - 1.0f) / (sinTheta + 1.0f);

        for (int i = 0; i < stages; ++i) {
            apfCoeffs[i] = coeff;
        }
    }

    void process(float* buffer, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            float y = buffer[i];

            for (int stage = 0; stage < stages; ++stage) {
                float x = y - apfBuffers[stage] * apfCoeffs[stage];
                float apfOut = apfBuffers[stage] + apfCoeffs[stage] * x;
                apfBuffers[stage] = x;
                y = apfOut;
            }

            buffer[i] = y;
        }
    }

private:
    int stages;
    std::vector<float> apfBuffers;
    std::vector<float> apfCoeffs;
};