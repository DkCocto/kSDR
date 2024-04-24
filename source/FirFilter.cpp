#include "FirFilter.h"

FirFilter::FirFilter(std::vector<float> taps, int tabsLen) {
	coeffs = taps;
	M = tabsLen - 1;
	xv = std::vector<float>(M + 1, 0);
	xv.reserve(M + 1);
}

float FirFilter::filter(float in) {
	float sum = 0.0f;
	int i;
	for (i = 0; i < M; i++) xv[i] = xv[i + 1];
	xv[M] = in;
	for (i = 0; i <= M; i++) {
		sum += (coeffs[i] * xv[i]);
	}
	return sum;
}