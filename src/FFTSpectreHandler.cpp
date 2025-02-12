#include "FFTSpectreHandler.h"
#include "FPSControl.cpp"

using namespace std::chrono;

FPSControl* fpsControl;

WindowHandler wh;

SpectreHandler::SpectreHandler(Config* config, FFTData* fftData, ViewModel* viewModel, DeviceController* deviceController) {
	this->config = config;
	this->viewModel = viewModel;

	this->fftData = fftData;
	this->deviceController = deviceController;

	spectreSize = config->fftLen / 2;

	//wb = new WindowBlackman(config->fftLen);
	//wbh = new WindowBlackmanHarris(config->fftLen);

	superOutput = new float[spectreSize];
	memset(superOutput, -100, sizeof(float) * spectreSize);

	outputWaterfall = new float[spectreSize];
	memset(outputWaterfall, -100, sizeof(float) * spectreSize);

	veryRawDataArray = new float[spectreSize];
	memset(veryRawDataArray, -100, sizeof(float) * spectreSize);

	spectreRawArray = new float[spectreSize];
	memset(spectreRawArray, -100, sizeof(float) * spectreSize);

	spectreRaw2Array = new float[spectreSize];
	memset(spectreRaw2Array, -100, sizeof(float) * spectreSize);

	tmpArray2 = new float[spectreSize];
	memset(tmpArray2, -100, sizeof(float) * spectreSize);

	inData = new fftw_complex[config->fftLen];
	outData = new fftw_complex[spectreSize];

	//FFTW_MEASURE
	printf("FFT module initializing...\n");
	fftwPlan = fftw_plan_dft_1d(spectreSize, inData, outData, FFTW_FORWARD, FFTW_MEASURE);

	speedDelta = new float[spectreSize];
	//memset(speedDelta, 1, sizeof(float) * spectreSize);

	fpsControl = new FPSControl(60.0 / (config->fftLen / 1024.0) + config->currentWorkingInputSamplerate / (config->fftLen / 2.0));

	wh.setLen(config->fftLen);
	wh.reinitializeWindow("Kaiser");
}

SpectreHandler::~SpectreHandler() {
	fftw_destroy_plan(fftwPlan);
	//delete wb;
	//delete wbh;
	delete fpsControl;
	delete[] superOutput;
	delete[] outputWaterfall;
	delete[] spectreRawArray;
	delete[] spectreRaw2Array;
	delete[] tmpArray2;
	delete[] inData;
	delete[] outData;
	delete[] speedDelta;
}

long long startTimeFpsControl = 0;
long long endTimeFpsControl = 0;
long long deltaFpsControl = 0;

void SpectreHandler::run() {
	isWorking_ = true;

	DeviceN* device = deviceController->getDevice();
	DeviceType deviceType = deviceController->getCurrentDeviceType();

	while (true) {
		if (!config->WORKING) {
			printf("SpectreHandler Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (device != nullptr) {
			switch (deviceType) {
				case HACKRF:
					prepareToProcess<HackRFDevice, uint8_t>((HackRFDevice*)device);
					break;
				case RTL:
					prepareToProcess<RTLDevice, unsigned char>((RTLDevice*)device);
					break;
				case RSP:
					prepareToProcess<RSPDevice, short>((RSPDevice*)device);
					break;
				case SOUNDCARD:
					prepareToProcess<SoundCardDevice, float>((SoundCardDevice*)device);
					break;
			}
		} else std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

template<typename DEVICE, typename DATATYPE> void SpectreHandler::prepareToProcess(DEVICE* device) {
	auto buffer = device->getBufferForSpec();
	if (buffer->available() >= config->fftLen) {
		processFFT<DATATYPE, DEVICE>(buffer->read(), device);
		//fpsControl->tick();
	} else std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

int oldMin = 0;
int oldMax = 0;

template<typename T, typename D> void SpectreHandler::processFFT(T* data, D* device) {
	for (int i = 0; i < spectreSize; i++) {
		
		float I = device->prepareData(data[2 * i]);
		float Q = device->prepareData(data[2 * i + 1]);

		if (viewModel->removeDCBias) dcRemove.process(&I, &Q);

		inData[i][0] = I * wh.getWeights()[i];
		inData[i][1] = Q * wh.getWeights()[i];
	}

	fftw_execute(fftwPlan);

	dataPostprocess();

	if (config->TRANSMITTING) {

		float* outputCopy = new float[spectreSize];

		memcpy(outputCopy, superOutput, sizeof(superOutput) * spectreSize);

		for (int i = config->receiver.receiveBinA; i <= config->receiver.receiveBinB; i++) {
			if (oldMax < outputCopy[i]) oldMax = outputCopy[i];
			if (oldMin > outputCopy[i]) oldMin = outputCopy[i];
		}

		for (int i = 0; i < spectreSize; i++) {
			if (i < config->receiver.receiveBinA || i > config->receiver.receiveBinB) {
				outputCopy[i] = config->spectre.minVisibleDB;
			}
			else {
				outputCopy[i] = Utils::convertSegment(outputCopy[i], oldMin, oldMax, config->spectre.minVisibleDB, config->spectre.maxVisibleDB);
			}
		}
		fftData->setData(outputCopy, outputCopy, spectreSize);

		delete[] outputCopy;
	} else {
		fftData->setData(superOutput, outputWaterfall, spectreSize);
	}
}
	
void smoothSpectre(float* src, int size, int gaussDepth) {
	const float kernel[5] = { 0.06f, 0.24f, 0.40f, 0.24f, 0.06f };
	float* temp = new float[size];

	// 🔹 Гауссово сглаживание (повторяем несколько раз для глубины)
	for (int iter = 0; iter < gaussDepth; iter++) {
		for (int i = 2; i < size - 2; i++) {
			temp[i] = src[i - 2] * kernel[0] +
				src[i - 1] * kernel[1] +
				src[i] * kernel[2] +
				src[i + 1] * kernel[3] +
				src[i + 2] * kernel[4];
		}
		// Копируем обратно в data
		for (int i = 2; i < size - 2; i++) {
			src[i] = temp[i];
		}
	}

	delete[] temp;
}

float SpectreHandler::average(float avg, float new_sample, int n) {
	return avg + (new_sample - avg) / (float)n;
}

void SpectreHandler::calculateAndNormalizeBinInRawData(float* data, int size) {
	for (int i = 0; i < size; i++) {
		data[Utils::convFFTResBinToSpecBin(i, size)] = this->psd(outData[i][0], outData[i][1]) + config->spectre.spectreCorrectionDb;
	}
}

void SpectreHandler::dataPostprocess() {
	calculateAndNormalizeBinInRawData(veryRawDataArray, spectreSize);

	for (int i = 0; i < spectreSize; i++) {
		spectreRawArray[i] = average(spectreRawArray[i], veryRawDataArray[i], 2);
		outputWaterfall[i] = spectreRawArray[i];
	}

	smoothSpectre(spectreRawArray, spectreSize, config->spectre.smoothingDepth);

	for (int i = 0; i < spectreSize; i++) {
		spectreRaw2Array[i] = average(spectreRaw2Array[i], spectreRawArray[i], config->spectre.spectreSpeed);
	}

	for (int i = 0; i < spectreSize; i++) {
		if (config->spectre.hangAndDecay) {
			if (superOutput[i] < spectreRaw2Array[i]) {
				superOutput[i] = spectreRaw2Array[i];
				speedDelta[i] = 1.0f;
			}
			else {
				superOutput[i] -= config->spectre.decaySpeed * speedDelta[i];
				speedDelta[i] += config->spectre.decaySpeedDelta;
			}
		}
		else {
			superOutput[i] = spectreRaw2Array[i];
		}
	}
}

float SpectreHandler::psd(float re, float im) {
	float basis = sqrt((re * re + im * im) / (float)config->fftLen);
	if (basis == 0) basis = 0.0001;
	return 10.0f * log(basis);
}

FFTData* SpectreHandler::getFFTData() {
	return fftData;
}

std::thread SpectreHandler::start() {
	std::thread p(&SpectreHandler::run, this);
	return p;
}