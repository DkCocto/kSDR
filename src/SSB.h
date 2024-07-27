#pragma once

#include "Config.h"
#include "Modulation.h"
#include "HilbertTransformFFTW.h"
#include "ComplexSignal.h"
#include "ComplexOscillator.h"
#include "Mixer.h"
#include "WindowBlackmanHarris.h"
#include "SinOscillator.h"

#define SSB_HILBERT_TRANSFORM_LEN 128 * 1024

class SSBModulation : Modulation {

private:

	int inputDataLen;
	int outputDataLen;

	const int carierFreq = 50000;

	Config* config = nullptr;

	//double* inputDataCopyDouble;
	float* ssbData;
	//Signal* outputDataSignal;
	//float* outputData;
	float* inputData;

	HilbertTransformFFTW* hilbertTransformFFTW = nullptr;

	ComplexOscillator* carierSignal = nullptr;

	Mixer* mixer = nullptr;

	//WindowBlackmanHarris* windowBlackmanHarris;

	void init();

	SinOscillator so;

public:

	DataStruct* dataStruct;

	SSBModulation();
	~SSBModulation();

	void setConfig(Config* config);
	DataStruct* processData(CircleBufferNew<float>* buffer, int maxBufLen);
	int getInputBufferLen();
	int getOutputBufferLen();
	int getOutputBufferHalfLen();
	//int setOutputBufferLen(int outputBufferLen);
	void setFreq(int freq);

};