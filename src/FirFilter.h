#pragma once

#include <vector>
#include <cstring> // Для memmove

class FirFilter {
	std::vector<float> coeffs;
	std::vector<float> xv;
	int M;

public:
	FirFilter(std::vector<float> taps, int tapsLen) {
		coeffs = taps;
		M = tapsLen - 1;
		xv.resize(M + 1, 0.0f);
	}

	float filter(float in) {
		std::memmove(&xv[0], &xv[1], M * sizeof(float)); // Сдвигаем буфер
		xv[M] = in; // Новый входной отсчет

		float sum = 0.0f;
		for (int i = 0; i <= M; i++) {
			sum += coeffs[i] * xv[i];
		}
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