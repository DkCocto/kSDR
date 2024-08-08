#include "AM.h"

void AMModulation::setFreq(int freq) {
	if (this->freq != freq) {
		this->freq = freq;
		mixer->setFreq(freq);
	}
}

void AMModulation::initFilters() {
	if (this->storedFilterWidth != config->filterWidth) {
		this->storedFilterWidth = config->filterWidth;

		audioFilter.init(audioFilter.LOWPASS,
			audioFilter.BLACKMAN_NUTTAL,
			127,
			config->filterWidth,
			0,
			config->inputSamplerateSound);

		if (fastfir != nullptr) delete fastfir;
		if (fastfir2 != nullptr) delete fastfir2;

		//create FastFIR filter
		fastfir = new JFastFIRFilter;
		fastfir2 = new JFastFIRFilter;

		fastfir->setKernel(JFilterDesign::LowPassHanning(
			config->filterWidth,
			config->currentWorkingInputSamplerate,
			1111));

		fastfir2->setKernel(JFilterDesign::LowPassHanning(
			4 * config->filterWidth,
			config->currentWorkingInputSamplerate,
			1111));

	}
}

AMModulation::AMModulation(Config* config, int inputDataLen) {
	this->config = config;
	this->inputDataLen = inputDataLen;

	amData = new float[inputDataLen];

	relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound;
	upsamplingDataLen = inputDataLen * relation;

	signalData = new DataStruct;
	signalData->len = upsamplingDataLen * 2;
	signalData->data = new float[upsamplingDataLen * 2];
	memset(signalData->data, 0, sizeof(float) * upsamplingDataLen * 2);

	amQ = std::vector<float>(inputDataLen); amQ.reserve(inputDataLen);
	amI = std::vector<float>(inputDataLen); amI.reserve(inputDataLen);

	upsampledData = std::vector<double>(upsamplingDataLen); upsampledData.reserve(upsamplingDataLen);

	upsamplingDataQ = std::vector<double>(upsamplingDataLen); upsamplingDataQ.reserve(upsamplingDataLen);
	upsamplingDataI = std::vector<double>(upsamplingDataLen); upsamplingDataI.reserve(upsamplingDataLen);

	int hTransform1Len = 127;
	hilbertTransform1 = new HilbertTransform(config->inputSamplerateSound, hTransform1Len);
	delay1 = new Delay((hTransform1Len - 1) / 2);

	cosOscillator = new CosOscillator(0, config->inputSamplerateSound);
	mixer = new Mixer(config->currentWorkingInputSamplerate);
}

AMModulation::~AMModulation() {
	delete[] amData;
	delete signalData->data;
	delete signalData;
	delete hilbertTransform1;
	delete delay1;

	delete mixer;
	delete cosOscillator;
}

AMModulation::DataStruct* AMModulation::processData(float* input) {
	setFreq(config->transmit.txFreq);
	initFilters();

	for (int i = 0; i < inputDataLen; i++) {
		float oscillatorSignal = cosOscillator->nextSample();
		amData[i] = (1.0f + config->transmit.amModulationDepth * input[i]) * oscillatorSignal * config->transmit.inputLevel;
		//if (max < abs(amSignal)) max = abs(amSignal);
		//if (min > abs(amSignal)) min = abs(amSignal);
	}

	/*for (int i = 0; i < inputDataLen; i++) {
		amQ[i] = delay1->filter(input[i]);
		amI[i] = hilbertTransform1->filter(input[i]);
	}*/

	for (int i = 0; i < upsamplingDataLen; i++) {
		if (i % relation != 0) {
			double dither1 = ((double)rand() / (double)(RAND_MAX)) / 1000;
			upsamplingDataQ[i] = dither1;
			/*double dither2 = ((double)rand() / (double)(RAND_MAX)) / 1000;
			upsamplingDataI[i] = dither2;*/
		}
	}

	for (int i = 0; i < inputDataLen; i++) {
		//input[i] /= config->transmit.inputLevel3;
		upsamplingDataQ[i * relation] = amData[i];
		//upsamplingDataI[i * relation] = amI[i];
	}

	fastfir->Update(upsamplingDataQ);
	//fastfir2->Update(upsamplingDataI);

	for (int i = 0; i < upsamplingDataLen; i++) {
		float ditherI = ((float)rand() / (float)(RAND_MAX)) / 1000.0f;
		float ditherQ = ((float)rand() / (float)(RAND_MAX)) / 1000.0f;
		auto mixedSignal = mixer->mix(upsamplingDataQ[i], 0.0f + ditherQ);

		signalData->data[2 * i] = (mixedSignal.Q + ditherQ);
		signalData->data[2 * i + 1] = (mixedSignal.I + ditherI);
	}

	return signalData;
}