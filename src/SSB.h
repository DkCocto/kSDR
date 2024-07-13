#pragma once

#include "Config.h"
#include "Modulation.h"
#include "HilbertTransformFFTW.h"
#include "ComplexSignal.h"
#include "ComplexOscillator.h"
#include "Mixer.h"
#include "WindowBlackmanHarris.h"

#define SSB_HILBERT_TRANSFORM_LEN 128 * 1024

class SSBModulation : Modulation {

private:

	int inputDataLen = 512;

	const int carierFreq = 100000;

	Config* config = nullptr;

	double* inputDataCopyDouble;
	double* ssbData;
	Signal* outputDataSignal;
	float* outputData;
	float* inputData;

	HilbertTransformFFTW* hilbertTransformFFTW = nullptr;

	ComplexOscillator* carierSignal = nullptr;

	Mixer* mixer = nullptr;

	WindowBlackmanHarris* windowBlackmanHarris;

	void init();

public:

	SSBModulation();
	~SSBModulation();

	void setConfig(Config* config);
	Signal* processData(CircleBufferNew<float>* buffer);
	int getInputBufferLen();
	int getOutputBufferLen();
	int getOutputBufferHalfLen();
	//int setOutputBufferLen(int outputBufferLen);
	void setFreq(int freq);

};