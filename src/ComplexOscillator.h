#pragma once

#include "SinOscillator.h"
#include "CosOscillator.h"
#include "ComplexSignal.h"

class ComplexOscillator {

	int freq = 0;
	int samplingRate = 0;

	CosOscillator* cosOscillator = nullptr;
	SinOscillator* sinOscillator = nullptr;

public:

	ComplexOscillator(int freq, int samplingRate);

	void setFreq(int freq);

	ComplexSignal* next();

};