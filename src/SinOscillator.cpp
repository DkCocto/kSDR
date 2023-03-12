#include "SinOscillator.h"

double SinOscillator::nextSample() {
	double value = fm.myFastSin((float)Oscillator::nextPhase());
	return value;
}