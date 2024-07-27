#include "SSB.h"

SSBModulation::SSBModulation() {

	this->inputDataLen = SSB_HILBERT_TRANSFORM_LEN;
	inputData = new float[inputDataLen];
	//inputDataCopyDouble = new double[inputDataLen];
	ssbData = new float[inputDataLen];
	outputDataSignal = new Signal[halfOutputBufferLen];
	outputData = new float[outputBufferLen];

	hilbertTransformFFTW = new HilbertTransformFFTW(SSB_HILBERT_TRANSFORM_LEN);

	windowBlackmanHarris = new WindowBlackmanHarris(inputDataLen * 2);
}

SSBModulation::~SSBModulation() {
	//delete[] inputDataCopyDouble;
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

Signal* SSBModulation::processData(CircleBufferNew<float>* buffer) {
	/*for (int i = 0; i < halfOutputBufferLen; i++) {
		auto signal = carierSignal->next();
		auto mixedSignal = mixer->mix(signal.I, signal.Q);
		outputDataSignal[i] = mixedSignal;
	}*/
	
	//                    131072               / 131072  = 1
	for (int j = 0; j < halfOutputBufferLen / inputDataLen; j++) {

		buffer->read(inputData, inputDataLen);

		//std::copy(inputData, inputData + inputDataLen, inputDataCopyDouble);

		fftw_complex* complex = hilbertTransformFFTW->process(inputData);
		
		for (int i = 0; i < inputDataLen; i++) {
			ComplexSignal carier = carierSignal->next();

			float dither = ((float)rand() / (float)(RAND_MAX)) / 250.0f;
			//float dither2 = ((float)rand() / (float)(RAND_MAX)) / 250.0f;


			float Q = (complex[i][REAL] + dither);
			float I = (complex[i][IMAG] + dither);
			
			if (config->receiver.modulation == USB) ssbData[i] = carier.I * Q + carier.Q * I;
			if (config->receiver.modulation == LSB) ssbData[i] = carier.I * Q - carier.Q * I;

		}

		hilbertTransformFFTW->process(ssbData);

		for (int i = 0; i < inputDataLen; i++) {
			float Q = complex[i][REAL];
			float I = complex[i][IMAG];
			//float dither = ((float)rand() / (float)(RAND_MAX)) / 250.0f;
			//float dither2 = ((float)rand() / (float)(RAND_MAX)) / 250.0f;
			auto mixedSignal = mixer->mix(Q, I);

			outputDataSignal[j * inputDataLen + i] = Signal{ mixedSignal.Q , mixedSignal.I };
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