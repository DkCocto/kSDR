#include "Mixer.h"

Mixer::Mixer(int inputSampleRate) {
	complexOscillator = new ComplexOscillator(22000, inputSampleRate);
}

void Mixer::setFreq(int freq) {
	if (this->freq != freq) {
		//cout << "SISYAKA " << this->freq << " " << freq << "\r\n";
		this->freq = freq;
		complexOscillator->setFreq(freq);
	}
}

Signal Mixer::mix(double I, double Q) {
	complexSignal = complexOscillator->next();
	complexSignal->normalize();
	Signal signal{ I * complexSignal->I + Q * complexSignal->Q , Q * complexSignal->I - I * complexSignal->Q };
	delete complexSignal;
	return signal;
}