#pragma once
#include "Oscillator.h"
#include "cmath"

class CosOscillator : public Oscillator {
public:

	CosOscillator(int freq, int samplingRate) : Oscillator(freq, samplingRate) { }

	double nextSample();
};