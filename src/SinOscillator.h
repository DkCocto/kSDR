#pragma once

#include "Oscillator.h"
#include "cmath"
#include "FastMath.h"

class SinOscillator : public Oscillator {

	FastMath fm;

public:

	SinOscillator() : Oscillator() {}
	SinOscillator(int freq, int samplingRate) : Oscillator(freq, samplingRate) {}

	float nextSample();
};