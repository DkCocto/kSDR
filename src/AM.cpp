#include "AM.h"

void AMModulation::setFreq(int freq) {
	if (this->freq != freq) {
		this->freq = freq;
		mixer->setFreq((freq + carierFreq));
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

		int downFreq = 0;
		int upFreq = 0;

		downFreq = carierFreq - config->filterWidth;
		upFreq = carierFreq + config->filterWidth;

		if (upsampledDataFilterQ != nullptr) delete upsampledDataFilterQ;
		if (upsampledDataFilterI != nullptr) delete upsampledDataFilterI;

		upsampledDataFilterQ = new JFastFIRFilter;
		upsampledDataFilterQ->setKernel(JFilterDesign::BandPassHanning(
			downFreq,
			upFreq,
			config->currentWorkingInputSamplerate,
			1555));

		upsampledDataFilterI = new JFastFIRFilter;
		upsampledDataFilterI->setKernel(JFilterDesign::BandPassHanning(
			downFreq,
			upFreq,
			config->currentWorkingInputSamplerate,
			1555));

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

	upsampledDataQ = std::vector<double>(upsamplingDataLen); upsampledDataQ.reserve(upsamplingDataLen);
	upsampledDataI = std::vector<double>(upsamplingDataLen); upsampledDataI.reserve(upsamplingDataLen);



	int hTransform1Len = 127;
	hilbertTransform1 = new HilbertTransform(config->inputSamplerateSound, hTransform1Len);
	delay1 = new Delay((hTransform1Len - 1) / 2);

	carierOscillator = new CosOscillator(carierFreq, config->inputSamplerateSound);

	mixer = new Mixer(config->currentWorkingInputSamplerate);
}

AMModulation::~AMModulation() {
	delete[] amData;
	delete signalData->data;
	delete signalData;
	delete hilbertTransform1;
	delete delay1;

	delete mixer;
	delete carierOscillator;
}

AMModulation::DataStruct* AMModulation::processData(float* input) {
	setFreq(config->transmit.txFreq);
	
	initFilters();


	//Генерируем АМ сигнал на заданной частоте и c определенной частоте дескретизации
	for (int i = 0; i < inputDataLen; i++) {
		float oscillatorSignal = carierOscillator->nextSample();
		amData[i] = (1.0f + config->transmit.amModulationDepth * audioFilter.proc(input[i])) * oscillatorSignal;

		amQ[i] = delay1->filter(amData[i]);
		amI[i] = hilbertTransform1->filter(amData[i]);

		//auto complexOscSignal = complexOscillator->next();
		//auto complexOscSignal = mixer->mix(am, 0);

		//signalData->data[2 * i] = complexOscSignal.Q * am;
		//signalData->data[2 * i + 1] = complexOscSignal.I * am;
	}

	//Обнуляем массив в котором будут храниться требуемые данные 
	std::fill(upsampledDataQ.begin(), upsampledDataQ.end(), 0.0);
	std::fill(upsampledDataI.begin(), upsampledDataI.end(), 0.0);


	for (int i = 0; i < inputDataLen; i++) {
		upsampledDataQ[i * relation] = amQ[i];
		upsampledDataI[i * relation] = amI[i];
	}

	upsampledDataFilterQ->Update(upsampledDataQ);
	upsampledDataFilterI->Update(upsampledDataI);

	for (int i = 0; i < upsamplingDataLen; i++) {
		auto complexOscSignal = mixer->mix(upsampledDataQ[i], upsampledDataI[i]);

		signalData->data[2 * i] = complexOscSignal.Q * config->transmit.inputLevel;
		signalData->data[2 * i + 1] = complexOscSignal.I * config->transmit.inputLevel;
	}


	return signalData;
}