#pragma once

#include "SinOscillator.h"
#include "CosOscillator.h"
#include "ComplexSignal.h"

class ComplexOscillator {

	int freq = 0;
	int samplingRate = 0;

	CosOscillator cosOscillator;
	SinOscillator sinOscillator;

public:

	ComplexOscillator();

	ComplexOscillator(int freq, int samplingRate);

	void setFreq(int freq);
	float getFreq();

	ComplexSignal next();

};