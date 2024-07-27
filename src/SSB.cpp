#include "SSB.h"

SSBModulation::SSBModulation() {
	this->inputDataLen = SSB_HILBERT_TRANSFORM_LEN;
	this->outputDataLen = 262144;

	inputData = new float[inputDataLen];
	
	dataStruct = new DataStruct;
	dataStruct->len = outputDataLen;
	dataStruct->data = new float[outputDataLen];

	ssbData = new float[inputDataLen];

	hilbertTransformFFTW = new HilbertTransformFFTW(SSB_HILBERT_TRANSFORM_LEN);
}

SSBModulation::~SSBModulation() {
	//delete[] inputDataCopyDouble;
	delete[] ssbData;
	delete[] inputData;

	delete hilbertTransformFFTW;

	if (dataStruct != nullptr) {
		if (dataStruct->data != nullptr) delete[] dataStruct->data;
		delete dataStruct;
	}

	if (carierSignal != nullptr) delete carierSignal;
}

void SSBModulation::init() {
	carierSignal = new ComplexOscillator(carierFreq, config->currentWorkingInputSamplerate);
	mixer = new Mixer(config->currentWorkingInputSamplerate);
}

void SSBModulation::setFreq(int freq) {
	this->freq = freq;
	mixer->setFreq((freq + carierFreq));
}

int SSBModulation::getOutputBufferLen() {
	return outputBufferLen;
}

int SSBModulation::getOutputBufferHalfLen() {
	return halfOutputBufferLen;
}

void SSBModulation::setConfig(Config* config) {
	this->config = config;
	init();
}

SSBModulation::DataStruct* SSBModulation::processData(CircleBufferNew<float>* buffer, int maxBufLen) {
							//256           64
	for (int j = 0; j < outputDataLen / (2 * inputDataLen); j++) {

		buffer->read(inputData, inputDataLen);

		fftw_complex* complex = hilbertTransformFFTW->process(inputData);

		for (int i = 0; i < inputDataLen; i++) {
			ComplexSignal carier = carierSignal->next();

			float Q = complex[i][REAL]; float I = complex[i][IMAG];

			if (config->receiver.modulation == USB) ssbData[i] = carier.I * Q + carier.Q * I;
			if (config->receiver.modulation == LSB) ssbData[i] = carier.I * Q - carier.Q * I;
		}

		hilbertTransformFFTW->process(ssbData);

		for (int i = 0; i < inputDataLen; i++) {
			float Q = complex[i][REAL]; float I = complex[i][IMAG];

			auto mixedSignal = mixer->mix(Q, I);

			dataStruct->data[j * 2 * inputDataLen + 2 * i] = mixedSignal.Q;
			dataStruct->data[j * 2 * inputDataLen + 2 * i + 1] = mixedSignal.I;
		}

	}
	
	return dataStruct;
}

int SSBModulation::getInputBufferLen() {
	return inputDataLen;
}