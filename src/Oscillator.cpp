#include "Oscillator.h"

double Oscillator::nextPhase() {
	phase += phaseIncrement;

	if (freq > 0.0 && phase >= 2.0 * M_PI) phase -= 2.0 * M_PI;
	if (freq < 0.0 && phase <= 0.0) phase += 2.0 * M_PI;

	return phase;
}

Oscillator::Oscillator() { }

Oscillator::Oscillator(int freq, int samplingRate) {
	this->freq = freq;
	this->samplingRate = samplingRate;
	init();
}

void Oscillator::setFreq(int freq) {
	this->freq = freq;
	init();
}

void Oscillator::init() {
	phase = 0.0;
	phaseIncrement = 2.0 * M_PI * (double)freq / (double)samplingRate;
}
