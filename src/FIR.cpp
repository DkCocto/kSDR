#include "FIR.h"

void FIR::init(unsigned char type, unsigned char window, short order, int f1, int f2, int sampleRate) {
    len = order;
    
    m_fir.clear();
    m_delay.clear();
    m_fir = std::vector<double>(order);
    m_delay = std::vector<double>(order);

    if (order == 1) {
        m_fir[0] = 1.0;
        return;
    }

    const int n2 = order / 2;
    double w = 2.0 * M_PI * (double)f1 / (double)sampleRate;
    double sum = 0;

    for (int i = 0; i < order; i++) {
        const int d = i - n2;
        m_fir[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
        sum += m_fir[i];
    }

    // нормализация
    for (int i = 0; i < order; i++) {
        m_fir[i] /= sum;
    }
    //

    if (type == LOWPASS) return;
    else if (type == HIGHPASS) {

        for (int i = 0; i < order; i++) {
            m_fir[i] = -m_fir[i];
        }

        m_fir[n2] += 1.0;

        return;

    } else {// если полосовой или режекторный фильтр

        // расчитываем верхнюю частоту
        double* hf = new double[order];

        w = 2.0 * M_PI * (double)f2 / (double)sampleRate;

        sum = 0;

        for (int i = 0; i < order; i++) {
            const int d = i - n2;
            hf[i] = ((d == 0) ? w : sin(w * d) / d) * getWindow(i, order, window);
            sum += hf[i];
        }

        // нормализация
        for (int i = 0; i < order; i++) {
            hf[i] /= sum;
        }

        // инвертируем и объединяем с ФНЧ
        for (int i = 0; i < order; i++) {
            m_fir[i] -= hf[i];
        }

        m_fir[n2] += 1.0;

        if (type == BANDSTOP) return;

        else if (type == BANDPASS) {
            for (int i = 0; i < order; i++) {
                m_fir[i] = -m_fir[i];
            }

            m_fir[n2] += 1.0;
        }
        delete[] hf;
    }
}

double FIR::getWindow(int i, int n, unsigned char window) {
    if (window == BARTLETT) {// устраняем нулевые значения
        double a = i - (n - 1) / 2.0;
        if (a < 0) a = -a;
        return 2.0 / n * (n / 2.0 - a);
    }
    else if (window == HANNING)// устраняем нулевые значения
        return 0.5 - 0.5 * cos(M_PI / n * (1.0 + 2.0 * i));
    if (window == BLACKMAN) {// устраняем нулевые значения
        double a = M_PI / n * (1.0 + 2.0 * i);
        return 0.5 * (1.0 - 0.16 - cos(a) + 0.16 * cos(2.0 * a));
    } else {
        double a = 2.0 * M_PI * i / (n - 1);

        if (window == HAMMING) return 0.54 - 0.46 * cos(a);
        else if (window == BLACKMAN_HARRIS) return 0.35875 - 0.48829 * cos(a) + 0.14128 * cos(2.0 * a) - 0.01168 * cos(3.0 * a);
        else if (window == BLACKMAN_NUTTAL) return 0.35819 - 0.4891775 * cos(a) + 0.1365995 * cos(2.0 * a) - 0.0106411 * cos(3.0 * a);

        else if (window == NUTTAL) return 0.355768 - 0.487396 * cos(a) + 0.144232 * cos(2.0 * a) - 0.012604 * cos(3.0 * a);

    }
    return 1.0;
}

double FIR::proc(double sample) {
    // линия задержки
    for (int i = len; --i > 0;) m_delay[i] = m_delay[i - 1];

    m_delay[0] = sample;

    // расчёт отклика
    double out = 0;
    for (int i = 0; i < len; i++) out += m_delay[i] * m_fir[i];

    // ограничение амплитуды
    /*if (out > 0) {
        out += 0.5;
        if (out > 32767) out = 32767;
    } else {
        out -= 0.5;
        if (out < -32768) out = -32768;
    }*/
    return out;
}

std::vector<double> FIR::getCoeffs() {
    return m_fir;
}