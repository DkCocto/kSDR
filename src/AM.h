#pragma once

#include "Modulation.h"
#include "HilbertTransformFFTW.h"
#include "Mixer.h"
#include "CosOscillator.h"

class AMModulation : Modulation {

private:

	Config* config = nullptr;

	int inputDataLen = 64 * 1024;

	float* inputData;
	float* amData;
	Signal* outputDataSignal;

	Mixer* mixer = nullptr;

	CosOscillator* cosOscillator = nullptr;

	void init();

public:

	AMModulation();
	~AMModulation();

	DataStruct* processData(CircleBufferNew<float>* buffer, int maxBufLen);
	void setFreq(int freq);
	void setConfig(Config* config);
	int getOutputBufferHalfLen();

};