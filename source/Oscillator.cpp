#include "Oscillator.h"

float Oscillator::nextPhase() {
	phase += phaseIncrement;

	if (freq > 0.0f && phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;
	if (freq < 0.0f && phase <= 0.0f) phase += 2.0f * M_PI;

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
	phase = 0.0f;
	phaseIncrement = 2.0f * M_PI * (float)freq / (float)samplingRate;
}
