#pragma once
#include "Oscillator.h"
#include "cmath"
#include "FastMath.h"

class CosOscillator : public Oscillator {

	FastMath fm;

public:

	CosOscillator() : Oscillator() { }

	CosOscillator(int freq, int samplingRate) : Oscillator(freq, samplingRate) { }

	double nextSample();
};