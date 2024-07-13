#include "SSB.h"

SSBModulation::SSBModulation() {
	this->inputDataLen = SSB_HILBERT_TRANSFORM_LEN;
	inputData = new float[inputDataLen];
	inputDataCopyDouble = new double[inputDataLen];
	ssbData = new double[inputDataLen];
	outputDataSignal = new Signal[halfOutputBufferLen];
	outputData = new float[outputBufferLen];

	hilbertTransformFFTW = new HilbertTransformFFTW(SSB_HILBERT_TRANSFORM_LEN);

	windowBlackmanHarris = new WindowBlackmanHarris(inputDataLen * 2);
}

SSBModulation::~SSBModulation() {
	delete[] inputDataCopyDouble;
	delete[] ssbData;
	delete[] outputDataSignal;
	delete[] outputData;
	delete[] inputData;

	delete hilbertTransformFFTW;

	if (carierSignal != nullptr) delete carierSignal;
}

void SSBModulation::init() {
	carierSignal = new ComplexOscillator(carierFreq, config->currentWorkingInputSamplerate);
	mixer = new Mixer(config->currentWorkingInputSamplerate);
}

void SSBModulation::setFreq(int freq) {
	this->freq = freq;
	mixer->setFreq(-freq - carierFreq);
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

Signal* SSBModulation::processData(CircleBufferNew<float>* buffer) {
	//                    131072               / 8192  = 16
	for (int j = 0; j < halfOutputBufferLen / inputDataLen; j++) {

		buffer->read(inputData, inputDataLen);

		std::copy(inputData, inputData + inputDataLen, inputDataCopyDouble);

		fftw_complex* complex = hilbertTransformFFTW->process(inputDataCopyDouble);
		
		for (int i = 0; i < inputDataLen; i++) {
			ComplexSignal carier = carierSignal->next();

			float I = (float)complex[i][IMAG];
			float Q = (float)complex[i][REAL];

			float ssb = 0;
			if (config->receiver.modulation == USB) ssb = carier.I * Q + carier.Q * I;
			if (config->receiver.modulation == LSB) ssb = carier.I * Q - carier.Q * I;

			ssbData[i] = ssb * config->transmit.inputLevel3;
		}

		fftw_complex* complex2 = hilbertTransformFFTW->process(ssbData);

		for (int i = 0; i < inputDataLen; i++) {
			float I = (float)complex2[i][IMAG];
			float Q = (float)complex2[i][REAL];
			float dither = ((float)rand() / (float)(RAND_MAX)) / 100.0f;
			float dither2 = ((float)rand() / (float)(RAND_MAX)) / 100.0f;
			auto mixedSignal = mixer->mix(I, Q);
			outputDataSignal[j * inputDataLen + i] = Signal{ mixedSignal.I + dither , mixedSignal.Q + dither2 };
		}

	}

	return outputDataSignal;

}

int SSBModulation::getInputBufferLen() {
	return inputDataLen;
}

/*int SSB::setOutputBufferLen(int outputBufferLen) {
	this->outputBufferLen = outputBufferLen;
	this->halfOutputBufferLen = outputBufferLen >> 1;
}*/