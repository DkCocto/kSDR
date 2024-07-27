#pragma once

#include "Env.h"
#include "Config.h"
#include "CircleBufferNew.h"
#include "SSB.h"
#include "AM.h"

class TransmittingData {

private:

	Config* config;

	CircleBufferNew<float>* txBuffer = nullptr;

	int outputBufLen = 262144;

	SSBModulation ssb;
	AMModulation am;

	Signal* emptySignals;

public:

	TransmittingData(Config* config, int sampleRate, int freq, int outputBufLen);
	~TransmittingData();

	Modulation::DataStruct* nextBuffer(int maxBufLen);
	void setFreq(int freq);
	void setTXBuffer(CircleBufferNew<float>* txBuffer);
	CircleBufferNew<float>* getTXBuffer();
};