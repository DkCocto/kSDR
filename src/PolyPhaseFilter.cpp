#include "PolyPhaseFilter.h"

void PolyPhaseFilter::initSubFilters(double* coeffs, int coeffsLen, int R) {
	int M = coeffsLen;
	subFilters = new std::vector<FirFilter>[R];
	int P = R - 1; // position of the polyphase switch

	for (int j = 0; j < R; j++) {
		int tabsLen = M / R;
		double* taps = new double[tabsLen];
		for (int i = 0; i < tabsLen; i++) {
			taps[i] = coeffs[P + i * R];
		}
		subFilters->push_back(FirFilter(taps, tabsLen));
		P = P - 1;
		if (P < 0) P = R - 1;
	}
}

PolyPhaseFilter::PolyPhaseFilter(double sampleRate, double freq, int decimationRate, int len) {
	coeffs = Filter::makeRaiseCosine(sampleRate, freq, alpha, len);
	printf("Raised Cosine PolyPhase Filter\r\n");
	initSubFilters(coeffs, len, decimationRate);
}

double PolyPhaseFilter::filter(double* in, int inLen) {
	double sum = 0.0;
	int j = inLen - 1;
	for (int i = 0; i < inLen; i++) sum += subFilters->at(i).filter(in[j--]);
	//for (int i = 0; i < inLen; i++) sum += subFilters->at(i).filter(in[j--]);
	//printf("Sum: %f\r\n", sum);
	return sum;
}