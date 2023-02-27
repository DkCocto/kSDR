#include "Mixer.h"

Mixer::Mixer(int inputSampleRate) {
	complexOscillator = new ComplexOscillator(22000, inputSampleRate);
}

void Mixer::setFreq(int freq) {
	if (this->freq != freq) {
		this->freq = freq;
		complexOscillator->setFreq(freq);
	}
}

Signal Mixer::mix(double I, double Q) {
	complexSignal = complexOscillator->next();
	complexSignal->normalize();
	Signal signal{ (float)(I * complexSignal->I + Q * complexSignal->Q) , (float)(Q * complexSignal->I - I * complexSignal->Q) };
	delete complexSignal;
	return signal;
}