#include "CosOscillator.h"

float CosOscillator::nextSample() {
	float value = fm.myFastCos(Oscillator::nextPhase());
	//float value = cos(Oscillator::nextPhase());
	return value;
} 