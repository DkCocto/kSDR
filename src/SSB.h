#pragma once

#include "Config.h"
#include "Modulation.h"
#include "HilbertTransformFFTW.h"
#include "ComplexSignal.h"
#include "ComplexOscillator.h"
#include "Mixer.h"
#include "CircleBufferNew.h"
#include "WindowBlackmanHarris.h"

#define SSB_HILBERT_TRANSFORM_LEN 32768

class SSB : Modulation {

private:

	int inputDataLen = 1024;
	const int outputBufferLen = 262144;
	const int halfOutputBufferLen = 262144 >> 1;

	const int carierFreq = 1000000;

	Config* config = nullptr;

	double* inputDataCopyDouble;
	double* ssbData;
	Signal* outputDataSignal;
	float* outputData;
	float* inputData;

	HilbertTransformFFTW* hilbertTransformFFTW = nullptr;
	HilbertTransformFFTW* hilbertTransformFFTW1 = nullptr;

	ComplexOscillator* carierSignal = nullptr;

	Mixer* mixer = nullptr;

	WindowBlackmanHarris* windowBlackmanHarris;

	void init();

public:

	SSB();
	~SSB();

	void setConfig(Config* config);
	Signal* processData(CircleBufferNew<float>* buffer);
	int getInputBufferLen();
	int getOutputBufferLen();
	//int setOutputBufferLen(int outputBufferLen);
	void setFreq(int freq);

};