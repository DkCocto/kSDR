#pragma once

#include "Oscillator.h"
#include "cmath"

class SinOscillator : public Oscillator {
public:
	SinOscillator(int freq, int samplingRate) : Oscillator(freq, samplingRate) {}

	double nextSample();
};