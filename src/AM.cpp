#include "AM.h"

void AMModulation::init() {
	mixer = new Mixer(config->currentWorkingInputSamplerate);
	cosOscillator = new CosOscillator(0, config->currentWorkingInputSamplerate);
}

void AMModulation::setFreq(int freq) {
	this->freq = freq;
	mixer->setFreq(-freq);
}

AMModulation::AMModulation() {
	inputData = new float[inputDataLen];
	outputDataSignal = new Signal[halfOutputBufferLen];
	amData = new float[inputDataLen];
}

AMModulation::~AMModulation() {
	delete[] inputData;
	delete[] outputDataSignal;
	delete[] amData;
}

void AMModulation::setConfig(Config* config) {
	this->config = config;
	init();
}

int AMModulation::getOutputBufferHalfLen() {
	return halfOutputBufferLen;
}

AMModulation::DataStruct* AMModulation::processData(CircleBufferNew<float>* buffer, int maxBufLen) {

	for (int j = 0; j < halfOutputBufferLen / inputDataLen; j++) {

		buffer->read(inputData, inputDataLen);

		float max = 0;

		for (int i = 0; i < inputDataLen; i++) {
			float oscillatorSignal = cosOscillator->nextSample();
			float amSignal = (1.0f + config->transmit.amModulationDepth * inputData[i]) * oscillatorSignal;
			amData[i] = amSignal;
			if (max < abs(amSignal)) max = abs(amSignal);
		}

		for (int i = 0; i < inputDataLen; i++) {
			amData[i] /= max;
			float ditherI = ((float)rand() / (float)(RAND_MAX)) / 100000.0f;
			float ditherQ = ((float)rand() / (float)(RAND_MAX)) / 100000.0f;
			auto mixedSignal = mixer->mix(amData[i], 0.0f);
			outputDataSignal[j * inputDataLen + i] = Signal{ mixedSignal.I + ditherI, mixedSignal.Q + ditherQ };
		}

	}

	return new DataStruct;
}