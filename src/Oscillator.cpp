#include "Oscillator.h"

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
	phaseIncrement = (2 * M_PI / samplingRate) * freq;
}