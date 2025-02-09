#pragma once

/*#include <vector>
#include <immintrin.h> // AVX
#include <cmath>

class FIR {
public:
    enum WindowType { BARTLETT = 1, HANNING, HAMMING, BLACKMAN, BLACKMAN_HARRIS, BLACKMAN_NUTTAL, NUTTAL };
    enum FilterType { OFF = 0, LOWPASS, HIGHPASS, BANDSTOP, BANDPASS };

    std::vector<float> m_fir;
    std::vector<float> m_delay;
    int len = 0;
    int delayIndex = 0;

    ~FIR() = default;

    void init(FilterType type, WindowType window, short order, int f1, int f2, int sampleRate) {
        len = order;
        m_fir.resize(order);
        m_delay.resize(order, 0.0f);

        const int n2 = order / 2;
        float w = 2.0f * M_PI * static_cast<float>(f1) / static_cast<float>(sampleRate);
        float sum = 0.0f;

        for (int i = 0; i < order; i++) {
            const int d = i - n2;
            m_fir[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
            sum += m_fir[i];
        }

        for (int i = 0; i < order; i++) {
            m_fir[i] /= sum;
        }

        if (type == LOWPASS) return;
        else if (type == HIGHPASS) {
            for (int i = 0; i < order; i++) {
                m_fir[i] = -m_fir[i];
            }
            m_fir[n2] += 1.0f;
            return;
        }
        else {
            std::vector<float> hf(order);
            w = 2.0f * M_PI * static_cast<float>(f2) / static_cast<float>(sampleRate);
            sum = 0.0f;

            for (int i = 0; i < order; i++) {
                const int d = i - n2;
                hf[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
                sum += hf[i];
            }

            for (int i = 0; i < order; i++) {
                hf[i] /= sum;
                m_fir[i] -= hf[i];
            }

            m_fir[n2] += 1.0f;

            if (type == BANDPASS) {
                for (int i = 0; i < order; i++) {
                    m_fir[i] = -m_fir[i];
                }
                m_fir[n2] += 1.0f;
            }
        }
    }

    float getWindow(int i, int n, WindowType window) {
        float a = 2.0f * M_PI * i / (n - 1);
        switch (window) {
        case BARTLETT:
            return 1.0f - fabs((i - (n - 1) / 2.0f) / (n / 2.0f));
        case HANNING:
            return 0.5f - 0.5f * cos(a);
        case HAMMING:
            return 0.54f - 0.46f * cos(a);
        case BLACKMAN:
            return 0.42f - 0.5f * cos(a) + 0.08f * cos(2.0f * a);
        case BLACKMAN_HARRIS:
            return 0.35875f - 0.48829f * cos(a) + 0.14128f * cos(2.0f * a) - 0.01168f * cos(3.0f * a);
        case BLACKMAN_NUTTAL:
            return 0.3635819f - 0.4891775f * cos(a) + 0.1365995f * cos(2.0f * a) - 0.0106411f * cos(3.0f * a);
        case NUTTAL:
            return 0.355768f - 0.487396f * cos(a) + 0.144232f * cos(2.0f * a) - 0.012604f * cos(3.0f * a);
        default:
            return 1.0f;
        }
    }

    float proc(float sample) {
        m_delay[delayIndex] = sample;
        float out = 0.0f;

        int idx = delayIndex;
        int i = 0;
        __m256 sumVec = _mm256_setzero_ps();

        for (; i <= len - 8; i += 8) {
            __m256 delayVec = _mm256_loadu_ps(&m_delay[idx]);
            __m256 firVec = _mm256_loadu_ps(&m_fir[i]);
            sumVec = _mm256_fmadd_ps(delayVec, firVec, sumVec);
            idx = (idx + 8) % len;
        }

        float sumArray[8];
        _mm256_storeu_ps(sumArray, sumVec);
        out += sumArray[0] + sumArray[1] + sumArray[2] + sumArray[3] + sumArray[4] + sumArray[5] + sumArray[6] + sumArray[7];

        for (; i < len; i++) {
            out += m_delay[idx] * m_fir[i];
            idx = (idx + 1) % len;
        }

        delayIndex = (delayIndex + 1) % len;
        return out;
    }
};*/

#include "Env.h"

class FIR {

public:

	const unsigned char BARTLETT = 1;
    const unsigned char HANNING = 2;// Hanning window
    const unsigned char HAMMING = 3;// Hamming window
    const unsigned char BLACKMAN = 4;// Blackman window
    const unsigned char BLACKMAN_HARRIS = 5;// Blackman-Harris window
    const unsigned char BLACKMAN_NUTTAL = 6;// Blackman-Nuttal window
    const unsigned char NUTTAL = 7;// Nuttal window
    

    const unsigned char OFF = 0;
    const unsigned char LOWPASS = 1;
    const unsigned char HIGHPASS = 2;
    const unsigned char BANDSTOP = 3;// NOTCH
    const unsigned char BANDPASS = 4;

    std::vector<float> m_delay;
    std::vector<float> m_fir;

    int len = 0;

    ~FIR();


    void init(unsigned char type, unsigned char window, short order, int f1, int f2, int sampleRate);


    float getWindow(int i, int n, unsigned char window);



    float proc(float sample);

    std::vector<float> getCoeffs();

    int getLen();

};