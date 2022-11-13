#include "FirFilter.h"
#include "Filter.h"

FirFilter::FirFilter() {
	/*coeffs = Filter::makeRaiseCosine(62500, 2800, 1, 128);

	M = 128 - 1;
	xv = new double[M + 1];
	memset(xv, 0, sizeof(double) * (M + 1));*/
	//Utils::printArray(xv, 32);
}

FirFilter::FirFilter(double* taps, int tabsLen) {
	coeffs = taps;
	M = tabsLen - 1;
	xv = new double[M + 1];
	memset(xv, 0, sizeof(double) * (M + 1));
}

double FirFilter::filter(double in) {
	double sum = 0.0;
	int i;
	for (i = 0; i < M; i++) xv[i] = xv[i + 1];
	xv[M] = in;
	for (i = 0; i <= M; i++) {
		sum += (coeffs[i] * xv[i]);
	}
	return sum;
}