#include "Oscillator.h"

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
	startPhase = 2.0 * M_PI * (double)freq;
	phase = 0.0;
	phaseIncrement = startPhase / (double)samplingRate;
	printf("%d\n", freq);
}