#include "SinOscillator.h"

float SinOscillator::nextSample() {
	float value = fm.myFastSin(Oscillator::nextPhase());
	//float value = sin(Oscillator::nextPhase());
	return value;
}