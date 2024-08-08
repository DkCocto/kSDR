#pragma once

#include "Env.h"
#include "Config.h"
#include "CircleBufferNew.h"

class TransmittingData {

private:

	Config* config;

	CircleBufferNew<float>* txBuffer = nullptr;

	int outputBufLen = 262144;

	float* data;

	//Signal* emptySignals;

public:

	TransmittingData(Config* config, int sampleRate, int freq, int outputBufLen);
	~TransmittingData();

	float* nextBuffer();
	//void setFreq(int freq);
	void setTXBuffer(CircleBufferNew<float>* txBuffer);
	void resetTXBuffer();
	CircleBufferNew<float>* getTXBuffer();
};