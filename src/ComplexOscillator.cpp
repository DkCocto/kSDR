#include "ComplexOscillator.h"

ComplexOscillator::ComplexOscillator(int freq, int samplingRate) {
	this->freq = freq;
	this->samplingRate = samplingRate;
	cosOscillator = new CosOscillator(freq, samplingRate);
	sinOscillator = new SinOscillator(freq, samplingRate);
}

void ComplexOscillator::setFreq(int freq) {
	this->freq = freq;
	cosOscillator->setFreq(freq);
	sinOscillator->setFreq(freq);
}

ComplexSignal* ComplexOscillator::next() {
	//cout << cos << " " << sin << "\r\n";
	return new ComplexSignal(cosOscillator->nextSample(), sinOscillator->nextSample());
}