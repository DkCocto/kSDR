#pragma once

#include "Env.h"

class Oscillator {

public:

	int samplingRate = 0;

	int freq = 0;

	double startPhase = 0.0;

	double phase = 0.0;
	double phaseIncrement = 0.0;

	Oscillator();

	Oscillator(int freq, int samplingRate);

	void setFreq(int freq);

	void init();

	virtual double nextSample() = 0;
};