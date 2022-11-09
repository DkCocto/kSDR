#pragma once

#include "Env.h"

class Oscillator {
	int freq = 0;
	int samplingRate = 0;

public:

	double phase = 0.0;
	double phaseIncrement = 0.0;

	Oscillator(int freq, int samplingRate);

	void setFreq(int freq);

	void init();

	virtual double nextSample() = 0;
};