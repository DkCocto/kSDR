#include "PolyPhaseFilter.h"
#include "FIR.h"

void PolyPhaseFilter::initSubFilters(std::vector<float> coeffs, int coeffsLen, int R) {
	int M = coeffsLen;

	subFilters.clear();

	//subFilters = new std::vector<FirFilter>[R];
	int P = R - 1; // position of the polyphase switch

	for (int j = 0; j < R; j++) {
		int tabsLen = M / R;
		std::vector<float> taps(tabsLen);
		for (int i = 0; i < tabsLen; i++) {
			taps[i] = coeffs[P + i * R];
		}
		subFilters.push_back(FirFilter(taps, tabsLen));
		P = P - 1;
		if (P < 0) P = R - 1;
	}
}

void PolyPhaseFilter::initCoeffs(float sampleRate, float freq, int decimationRate, int len) {
	coeffs.clear();
	FIR fir;
	fir.init(fir.LOWPASS, fir.HAMMING, len, freq, 0, sampleRate);
	
	float* c = fir.getCoeffs();
	std::copy(&c[0], &c[fir.getLen()], std::back_inserter(coeffs));

	//coeffs = Filter::makeRaiseCosine(sampleRate, freq, alpha, len);

	printf("PolyPhase Filter. Len: %d\r\n", coeffs.size());
	initSubFilters(coeffs, len, decimationRate); 
}

PolyPhaseFilter::PolyPhaseFilter() { }

float PolyPhaseFilter::filter(float* in, int inLen) {
	float sum = 0.0;
	int j = inLen - 1;
	for (int i = 0; i < inLen; i++) {
		sum += subFilters.at(i).filter(in[j--]);
	}
	return sum;
}