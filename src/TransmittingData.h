#pragma once

#include "Env.h"
#include "Config.h"
#include "CircleBufferNew.h"
#include "SSB.h"

class TransmittingData {

private:

	Config* config;

	CircleBufferNew<float>* txBuffer = nullptr;

	int outputBufLen = 262144;

	SSB ssb;

	Signal* emptySignals;

public:

	TransmittingData(Config* config, int sampleRate, int freq, int outputBufLen);
	~TransmittingData();

	Signal* nextBuffer();
	void setFreq(int freq);
	void setTXBuffer(CircleBufferNew<float>* txBuffer);
	CircleBufferNew<float>* getTXBuffer();
};