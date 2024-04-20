#pragma once

#include "ComplexOscillator.h"

class Mixer {
	ComplexOscillator complexOscillator;

	int freq = 0;

	ComplexSignal complexSignal;

public:

	Mixer() {};

	Mixer(int inputSampleRate);

	void setFreq(int freq);
	Signal mix(float I, float Q);

};