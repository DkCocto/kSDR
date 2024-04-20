#include "CosOscillator.h"

float CosOscillator::nextSample() {
	float value = fm.myFastCos(Oscillator::nextPhase());
	return value;
} 