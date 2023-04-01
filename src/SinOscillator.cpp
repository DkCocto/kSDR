#include "SinOscillator.h"

float SinOscillator::nextSample() {
	float value = fm.myFastSin(Oscillator::nextPhase());
	return value;
}