#pragma once
#include "Oscillator.h"
#include "cmath"

class CosOscillator : public Oscillator {
public:

	CosOscillator() : Oscillator() { }

	CosOscillator(int freq, int samplingRate) : Oscillator(freq, samplingRate) { }

	double nextSample();
};