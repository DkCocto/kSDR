#pragma once

#include <immintrin.h>
#include <cmath>
#include <cstring>

class HilbertTransform {
    float* xv = nullptr; // Буфер входных данных
    float* coeffs = nullptr; // Коэффициенты фильтра
    int samplingRate = 0;
    int len = 0;
    int M = 0;

public:
    HilbertTransform(int samplingRate, int len) {
        this->samplingRate = samplingRate;
        this->len = len;
        init();
        M = len - 1;
        xv = new float[len]();
    }

    ~HilbertTransform() {
        delete[] xv;
        delete[] coeffs;
    }

    void init() {
        auto* tempCoeffs = new float[len];
        float sumofsquares = 0.0f;

        for (int n = 0; n < len; n++) {
            if (n == len / 2) {
                tempCoeffs[n] = 0.0f;
            }
            else {
                tempCoeffs[n] = samplingRate / (M_PI * (n - (len - 1) / 2.0f)) * (1.0f - cos(M_PI * (n - (len - 1) / 2.0f)));
            }
            sumofsquares += tempCoeffs[n] * tempCoeffs[n];
        }

        float gain = sqrt(sumofsquares);
        coeffs = new float[len];

        for (int i = 0; i < len; i++) {
            coeffs[i] = tempCoeffs[len - i - 1] / gain;
        }

        delete[] tempCoeffs;
    }

    float filter(float val) {
        // Смещаем буфер без копирования (используем циклический буфер)
        memmove(xv, xv + 1, M * sizeof(float));
        xv[M] = val;

        // SIMD-ускорение: обрабатываем по 8 чисел за раз
        __m256 sumVec = _mm256_setzero_ps();
        int i = 0;

        for (; i <= M - 8; i += 8) {
            __m256 xvVec = _mm256_loadu_ps(&xv[i]);      // Загружаем 8 чисел из xv
            __m256 coeffVec = _mm256_loadu_ps(&coeffs[i]); // Загружаем 8 коэффициентов
            sumVec = _mm256_fmadd_ps(xvVec, coeffVec, sumVec);  // FMA: sum += xv * coeffs
        }

        // Просуммируем векторные значения
        float sumArray[8];
        _mm256_storeu_ps(sumArray, sumVec);
        float sum = sumArray[0] + sumArray[1] + sumArray[2] + sumArray[3] +
            sumArray[4] + sumArray[5] + sumArray[6] + sumArray[7];

        // Оставшиеся элементы (если len не делится на 8)
        for (; i <= M; i++) {
            sum += xv[i] * coeffs[i];
        }

        return sum;
    }
};

/*#include "Env.h"
#include "cmath"
#include "cstring"

class HilbertTransform {

    float* xv = nullptr; // This array holds the delayed values
    float* coeffs = nullptr;

    int samplingRate = 0;
    int len = 0;

    float gain = 1.0f;

    int M = 0;

public:

    HilbertTransform(int samplingRate, int len);

    ~HilbertTransform();

    void init();

    float filter(float val);
};*/