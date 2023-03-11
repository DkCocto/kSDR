#include "SinOscillator.h"

double SinOscillator::nextSample() {
	double value = fm.fastsin(phase);
	phase += phaseIncrement;
	//if (phase > startPhase || phase < -startPhase) phase = 0;
	return value;
}