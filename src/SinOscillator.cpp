#include "SinOscillator.h"

double SinOscillator::nextSample() {
	double value = sin(phase);
	phase += phaseIncrement;
	return value;
}