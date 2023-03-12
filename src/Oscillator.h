#pragma once

#include "Env.h"

class Oscillator {

protected:
	double nextPhase();

public:

	int samplingRate = 0;

	int freq = 0;

	double phase = 0.0;

	double phaseIncrement = 0.0;

	Oscillator();

	Oscillator(int freq, int samplingRate);

	void setFreq(int freq);

	void init();

	virtual double nextSample() = 0;
};