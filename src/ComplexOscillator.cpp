#include "ComplexOscillator.h"

ComplexOscillator::ComplexOscillator() { }

ComplexOscillator::ComplexOscillator(int freq, int samplingRate) {
	this->freq = freq;
	this->samplingRate = samplingRate;
	cosOscillator = CosOscillator(freq, samplingRate);
	sinOscillator = SinOscillator(freq, samplingRate);
}

void ComplexOscillator::setFreq(int freq) {
	this->freq = freq;
	cosOscillator.setFreq(freq);
	sinOscillator.setFreq(freq);
}

ComplexSignal ComplexOscillator::next() {
	return ComplexSignal(cosOscillator.nextSample(), sinOscillator.nextSample());
}