#include "FIR.h"

FIR::~FIR() {
}

void FIR::init(unsigned char type, unsigned char window, short order, int f1, int f2, int sampleRate) {
    len = order;
    
  

    m_fir.clear();
    m_delay.clear();
    m_fir = std::vector<float>(order);
    m_delay = std::vector<float>(order);

    if (order == 1) {
        m_fir[0] = 1.0f;
        return;
    }

    const int n2 = order / 2;
    float w = 2.0f * M_PI * (float)f1 / (float)sampleRate;
    float sum = 0.0f;

    for (int i = 0; i < order; i++) {
        const int d = i - n2;
        m_fir[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
        sum += m_fir[i];
    }

    // ������������
    for (int i = 0; i < order; i++) {
        m_fir[i] /= sum;
    }
    //

    if (type == LOWPASS) return;
    else if (type == HIGHPASS) {

        for (int i = 0; i < order; i++) {
            m_fir[i] = -m_fir[i];
        }

        m_fir[n2] += 1.0f;

        return;

    } else {// ���� ��������� ��� ����������� ������

        // ����������� ������� �������
        float* hf = new float[order];

        w = 2.0f * M_PI * (float)f2 / (float)sampleRate;

        sum = 0.0f;

        for (int i = 0; i < order; i++) {
            const int d = i - n2;
            hf[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
            sum += hf[i];
        }

        // ������������
        for (int i = 0; i < order; i++) {
            hf[i] /= sum;
        }

        // ����������� � ���������� � ���
        for (int i = 0; i < order; i++) {
            m_fir[i] -= hf[i];
        }

        delete[] hf;

        m_fir[n2] += 1.0f;

        if (type == BANDSTOP) return;

        else if (type == BANDPASS) {
            for (int i = 0; i < order; i++) {
                m_fir[i] = -m_fir[i];
            }

            m_fir[n2] += 1.0f;
        }

    }
}

float FIR::getWindow(int i, int n, unsigned char window) {
    if (window == BARTLETT) {// ��������� ������� ��������
        float a = i - (n - 1) / 2.0f;
        if (a < 0) a = -a;
        return 2.0f / n * (n / 2.0f - a);
    }
    else if (window == HANNING)// ��������� ������� ��������
        return 0.5f - 0.5f * cos(M_PI / n * (1.0f + 2.0f * i));
    if (window == BLACKMAN) {// ��������� ������� ��������
        float a = M_PI / n * (1.0f + 2.0f * i);
        return 0.5f * (1.0f - 0.16f - cos(a) + 0.16f * cos(2.0f * a));
    } else {
        float a = 2.0f * M_PI * i / (n - 1);

        if (window == HAMMING) return 0.54f - 0.46f * cos(a);
        else if (window == BLACKMAN_HARRIS) return 0.35875f - 0.48829f * cos(a) + 0.14128f * cos(2.0f * a) - 0.01168f * cos(3.0f * a);
        else if (window == BLACKMAN_NUTTAL) return 0.35819f - 0.4891775f * cos(a) + 0.1365995f * cos(2.0f * a) - 0.0106411f * cos(3.0f * a);

        else if (window == NUTTAL) return 0.355768f - 0.487396f * cos(a) + 0.144232f * cos(2.0f * a) - 0.012604f * cos(3.0f * a);

    }
    return 1.0f;
}

float FIR::proc(float sample) {
    // ����� ��������
    for (int i = len; --i > 0;) m_delay[i] = m_delay[i - 1];

    m_delay[0] = sample;

    // ������ �������
    float out = 0.0f;
    for (int i = 0; i < len; i++) out += m_delay[i] * m_fir[i];

    return out;
}

std::vector<float> FIR::getCoeffs() {
    return m_fir;
}

int FIR::getLen() {
    return len;
}
