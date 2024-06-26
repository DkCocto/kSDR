#pragma once

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

    /**
     * инициализация параметров
     *
     * @param type       - тип фильтра
     * @param window     - окно
     * @param order      - порядок фильтра
     * @param f1         - частота ФНЧ и ФВЧ фильтра
     * @param f2         - верхняя частота для полосового и режекторного фильтра
     * @param sampleRate - частота дискретизации
     */
    void init(unsigned char type, unsigned char window, short order, int f1, int f2, int sampleRate);

    /**
     * получить коэффициент заданного типа окна
     *
     * @param i      - индекс
     * @param n      - размер окна
     * @param window - тип окна
     * @return коэффициент
     */
    float getWindow(int i, int n, unsigned char window);

    /**
     * фильтрация
     *
     * @param sample - отсчёт звука
     * @return результат фильтрации
     */

    float proc(float sample);

    std::vector<float> getCoeffs();

    int getLen();

};