#include "CosOscillator.h"

double CosOscillator::nextSample() {
	double value = fm.myFastCos((float)Oscillator::nextPhase());
	return value;
} 