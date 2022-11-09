#include "CosOscillator.h"

double CosOscillator::nextSample() {
	double value = cos(phase);
	phase += phaseIncrement;
	return value;
}