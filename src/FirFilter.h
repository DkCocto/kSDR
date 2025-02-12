#pragma once

#include "vector"

class FirFilter {
    std::vector<float> coeffs; // Коэффициенты фильтра
    std::vector<float> xv; // Этот массив хранит отложенные значения
    int M; // Количество отсчетов (длина фильтра)
    int currentIndex = 0; // Текущий индекс для кольцевого буфера
    float Fc = 0.0f; // Будет установлено как cutoffFreq/SAMPLE_RATE

public:
    FirFilter(std::vector<float> taps, int tabsLen) {
        coeffs = taps;
        M = tabsLen - 1;
        xv = std::vector<float>(M + 1, 0.0f); // Инициализация буфера с нулями
    }

    /**
     * Рассчитывает результат с помощью свертки (конволюции).
     * Предполагается, что коэффициенты фильтра уже перевернуты, так как
     * новые значения добавляются в конец задерживающей линии xv.
     * Обратите внимание, что симметричные фильтры, такие как фильтры с поднятым косинусом,
     * не требуют переворота.
     * @param in Входное значение
     * @return Результат фильтрации
     */
    float filter(float in) {
        // Сохраняем новое значение во время на текущем индексе
        xv[currentIndex] = in;

        // Выполняем вычисление суммы свертки
        float sum = 0.0f;
        int idx = currentIndex;
        for (int i = 0; i <= M; i++) {
            sum += coeffs[i] * xv[idx];
            idx = (idx + 1) % (M + 1); // Индексация с использованием кольцевого буфера
        }

        // Обновляем currentIndex для следующего вызова
        currentIndex = (currentIndex + 1) % (M + 1);

        return sum;
    }
};

/*#pragma once

#include "vector"

class FirFilter {

	std::vector<float> coeffs;
	std::vector<float> xv;				  // This array holds the delayed values
	int M; // The number of taps, the length of the filter
	float Fc = 0.0f; // Will be set to cutoffFreq/SAMPLE_RATE; 

public:

	FirFilter(std::vector<float> taps, int tabsLen) {
		coeffs = taps;
		M = tabsLen - 1;
		xv = std::vector<float>(M + 1, 0);
		xv.reserve(M + 1);
	}

	/**
	 * Calculate the result with convolution.  This assumes that the filter kernel coeffs are already reversed as
	 * new values are added to the end of the delay line xv.  Note that a symmetrical filter such as a raised
	 * cosine does not need to be reversed.
	 * @param in
	 * @return
	 
	float filter(float in) {
		float sum = 0.0f;
		int i;
		for (i = 0; i < M; i++) xv[i] = xv[i + 1];
		xv[M] = in;
		for (i = 0; i <= M; i++) {
			sum += (coeffs[i] * xv[i]);
		}
		return sum;
	}
};*/