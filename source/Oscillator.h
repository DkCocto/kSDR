#pragma once

#include "Env.h"

class Oscillator {

protected:
	float nextPhase();

public:

	int samplingRate = 0;

	int freq = 0;

	float phase = 0.0;

	float phaseIncrement = 0.0;

	Oscillator();

	Oscillator(int freq, int samplingRate);

	void setFreq(int freq);

	void init();

	virtual float nextSample() = 0;
};