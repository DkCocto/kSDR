#include "SSB.h"

SSBModulation::SSBModulation(Config* config, int inputDataLen) {
	
	this->config = config;
	this->inputDataLen = inputDataLen;

	relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound;
	upsamplingDataLen = inputDataLen * relation;

	ssbSignalData = new DataStruct;
	ssbSignalData->len = upsamplingDataLen * 2;
	ssbSignalData->data = new float[upsamplingDataLen * 2];
	memset(ssbSignalData->data, 0, sizeof(float) * upsamplingDataLen * 2);

	bufQ = std::vector<float>(inputDataLen); bufQ.reserve(inputDataLen);
	bufI = std::vector<float>(inputDataLen); bufI.reserve(inputDataLen);

	upsamplingDataQ = std::vector<double>(upsamplingDataLen); upsamplingDataQ.reserve(upsamplingDataLen);
	upsamplingDataI = std::vector<double>(upsamplingDataLen); upsamplingDataI.reserve(upsamplingDataLen);

	int hTransform1Len = 127;
	hilbertTransform1 = new HilbertTransform(config->inputSamplerateSound, hTransform1Len);
	delay1 = new Delay((hTransform1Len - 1) / 2);

	int hTransform2Len = 127;
	hilbertTransform2 = new HilbertTransform(config->inputSamplerateSound, hTransform2Len);
	delay2 = new Delay((hTransform2Len - 1) / 2);
	
	carierSignal = ComplexOscillator(carierFreq, config->inputSamplerateSound);
	mixer = Mixer(config->currentWorkingInputSamplerate);

}

SSBModulation::~SSBModulation() {
	delete ssbSignalData->data;
	delete ssbSignalData;
	delete hilbertTransform1;
	delete delay1;
	delete hilbertTransform2;
	delete delay2;
	if (fastfir != nullptr) delete fastfir;
	if (fastfir2 != nullptr) delete fastfir2;
}

void SSBModulation::setFreq(int freq) {
	if (this->freq != freq) {
		this->freq = freq;
		mixer.setFreq((freq + carierFreq));
	}
}

void SSBModulation::initFilters() {
	if (this->storedFilterWidth != config->filterWidth || this->storedModulation != config->receiver.modulation) {
		this->storedFilterWidth = config->filterWidth;
		this->storedModulation = config->receiver.modulation;

		int downFreq = 0;
		int upFreq = 0;

		if (config->receiver.modulation == USB) {
			downFreq = 150 + carierFreq - config->filterWidth;
			upFreq = carierFreq;
		} else {
			downFreq = carierFreq - 150;
			upFreq = carierFreq + config->filterWidth;
		}

		audioFilter.init(audioFilter.BANDPASS, 
			audioFilter.BLACKMAN_NUTTAL, 
			127, 
			downFreq,
			upFreq,
			config->inputSamplerateSound);

		if (fastfir != nullptr) delete fastfir;
		if (fastfir2 != nullptr) delete fastfir2;
		//create FastFIR filter
		fastfir = new JFastFIRFilter;
		fastfir2 = new JFastFIRFilter;

		fastfir->setKernel(JFilterDesign::BandPassHanning(
			downFreq,
			upFreq,
			config->currentWorkingInputSamplerate,
			1111));

		fastfir2->setKernel(JFilterDesign::BandPassHanning(
			downFreq,
			upFreq,
			config->currentWorkingInputSamplerate,
			1111));

	}
}

SSBModulation::DataStruct* SSBModulation::processData(float* input) {

	initFilters();
	setFreq(config->transmit.txFreq);

	for (int i = 0; i < inputDataLen; i++) {

		//float audio = audioFilter.proc(input[i]) * config->transmit.inputLevel;
		float audio = input[i];

		float Q = delay1->filter(audio);
		float I = hilbertTransform1->filter(audio);

		ComplexSignal carier = carierSignal.next();
		if (config->receiver.modulation == USB) input[i] = carier.I * Q + carier.Q * I;
		if (config->receiver.modulation == LSB) input[i] = carier.I * Q - carier.Q * I;

		input[i] = audioFilter.proc(input[i]) * config->transmit.inputLevel;

		bufQ[i] = delay2->filter(input[i]);
		bufI[i] = hilbertTransform2->filter(input[i]);
	}


	for (int i = 0; i < upsamplingDataLen; i++) {
		if (i % relation != 0) {
			double dither1 = ((double)rand() / (double)(RAND_MAX)) / 10000000;
			double dither2 = ((double)rand() / (double)(RAND_MAX)) / 10000000;
			upsamplingDataQ[i] = dither1;
			upsamplingDataI[i] = dither2;
		}
	}

	for (int i = 0; i < inputDataLen; i++) {
		upsamplingDataQ[i * relation] = bufQ[i];
		upsamplingDataI[i * relation] = bufI[i];
	}

	fastfir->Update(upsamplingDataQ);
	fastfir2->Update(upsamplingDataI);

	//std::copy(upsamplingData.begin(), upsamplingData.end(), upsamplingDataOut);

	for (int i = 0; i < upsamplingDataLen; i++) {
		auto mixedSignal = mixer.mix(upsamplingDataQ[i], upsamplingDataI[i]);
		ssbSignalData->data[2 * i] = mixedSignal.Q;
		ssbSignalData->data[2 * i + 1] = mixedSignal.I;
	}

	return ssbSignalData;
}



/*buffer->read(dataStruct->data, dataStruct->len);

	for (int i = 0; i < dataStruct->len / 2; i++) {
		auto mixedSignal = mixer->mix(dataStruct->data[2 * i], dataStruct->data[2 * i + 1]);
		dataStruct->data[2 * i] = mixedSignal.Q;
		dataStruct->data[2 * i + 1] = mixedSignal.I;
	}*/

	//256           64
	/*for (int j = 0; j < outputDataLen / (2 * inputDataLen); j++) {

		buffer->read(inputData, inputDataLen);

		//fftw_complex* complex = hilbertTransformFFTW->process(inputData);

		/*for (int i = 0; i < inputDataLen; i++) {
			ComplexSignal carier = carierSignal->next();

			float Q = complex[i][REAL]; float I = complex[i][IMAG];

			if (config->receiver.modulation == USB) ssbData[i] = carier.I * Q + carier.Q * I;
			if (config->receiver.modulation == LSB) ssbData[i] = carier.I * Q - carier.Q * I;
		}*/

		/*fftw_complex* complex = hilbertTransformFFTW->process(inputData);

		for (int i = 0; i < inputDataLen; i++) {
			float Q = complex[i][REAL]; float I = complex[i][IMAG];

			auto mixedSignal = mixer->mix(inputData[i], inputData[i]);

			dataStruct->data[j * 2 * inputDataLen + 2 * i] = mixedSignal.Q;
			dataStruct->data[j * 2 * inputDataLen + 2 * i + 1] = mixedSignal.I;
		}*/

		//}*/