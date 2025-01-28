#include "FFTSpectreHandler.h"
#include "FPSControl.cpp"

using namespace std::chrono;

FPSControl* fpsControl;

SpectreHandler::SpectreHandler(Config* config, FFTData* fftData, ViewModel* viewModel, DeviceController* deviceController) {
	this->config = config;
	this->viewModel = viewModel;

	this->fftData = fftData;
	this->deviceController = deviceController;

	spectreSize = config->fftLen / 2;

	wb = new WindowBlackman(config->fftLen);
	wbh = new WindowBlackmanHarris(config->fftLen);

	superOutput = new float[spectreSize];
	memset(superOutput, -100, sizeof(float) * spectreSize);

	outputWaterfall = new float[spectreSize];
	memset(outputWaterfall, -100, sizeof(float) * spectreSize);

	tmpArray = new float[spectreSize];
	memset(tmpArray, -100, sizeof(float) * spectreSize);

	tmpArray2 = new float[spectreSize];
	memset(tmpArray, -100, sizeof(float) * spectreSize);

	inData = new fftw_complex[config->fftLen];
	
	outData = new fftw_complex[spectreSize];


	//FFTW_MEASURE
	printf("FFT module initializing...\n");
	fftwPlan = fftw_plan_dft_1d(spectreSize, inData, outData, FFTW_FORWARD, FFTW_MEASURE);

	speedDelta = new float[spectreSize];
	//memset(speedDelta, 1, sizeof(float) * spectreSize);

	fpsControl = new FPSControl(60.0 / (config->fftLen / 1024.0) + config->currentWorkingInputSamplerate / (config->fftLen / 2.0));
}

SpectreHandler::~SpectreHandler() {
	fftw_destroy_plan(fftwPlan);
	delete wb;
	delete wbh;
	delete fpsControl;
	delete[] superOutput;
	delete[] outputWaterfall;
	delete[] tmpArray;
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

		/*endTimeFpsControl = std::chrono::steady_clock::now().time_since_epoch().count() / 1000000; // получаем время в миллисекундах
		deltaFpsControl = endTimeFpsControl - startTimeFpsControl;

		if (deltaFpsControl < 14) {
			std::this_thread::sleep_for(std::chrono::milliseconds(14 - deltaFpsControl)); // пауза на оставшиеся миллисекунды
		}

		startTimeFpsControl = std::chrono::steady_clock::now().time_since_epoch().count() / 1000000; // обновляем время*/


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
			}


			

		} else std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

template<typename DEVICE, typename DATATYPE> void SpectreHandler::prepareToProcess(DEVICE* device) {
	auto buffer = device->getBufferForSpec();
	if (buffer->available() >= config->fftLen) {
		processFFT<DATATYPE, DEVICE>(buffer->read(), device);
		fpsControl->tick();
	} else std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

int oldMin = 0;
int oldMax = 0;

template<typename T, typename D> void SpectreHandler::processFFT(T* data, D* device) {
	for (int i = 0; i < spectreSize; i++) {
		
		float I = device->prepareData(data[2 * i]);
		float Q = device->prepareData(data[2 * i + 1]);

		if (viewModel->removeDCBias) dcRemove.process(&I, &Q);

		inData[i][0] = I * wb->getWeights()[i];
		inData[i][1] = Q * wb->getWeights()[i];
	}

	fftw_execute(fftwPlan);

	dataPostprocess();

	//config->TRANSMITTING
	if (config->TRANSMITTING) {

		float* outputCopy = new float[spectreSize];

		memcpy(outputCopy, superOutput, sizeof(superOutput) * spectreSize);

		for (int i = config->receiver.receiveBinA; i <= config->receiver.receiveBinB; i++) {
			i = Utils::convFFTResBinToSpecBin(i, spectreSize);
			if (oldMax < outputCopy[i]) oldMax = outputCopy[i];
			if (oldMin > outputCopy[i]) oldMin = outputCopy[i];
		}

		for (int i = 0; i < spectreSize; i++) {
			int newI = Utils::convFFTResBinToSpecBin(i, spectreSize);
			if (i < config->receiver.receiveBinA || i > config->receiver.receiveBinB) {
				outputCopy[newI] = config->spectre.minVisibleDB;
			}
			else {
				//if (abs(to2 - newMin) < config->spectre.maxVisibleDB) to2 = config->spectre.maxVisibleDB;
				outputCopy[newI] = Utils::convertSegment(outputCopy[newI], oldMin, oldMax, config->spectre.minVisibleDB, config->spectre.maxVisibleDB);
			}
		}
		fftData->setData(outputCopy, outputCopy, spectreSize);

		delete[] outputCopy;
	} else {
		fftData->setData(superOutput, outputWaterfall, spectreSize);
	}
}

float SpectreHandler::average(float avg, float new_sample, int n) {
	float tmp = avg;
	tmp -= avg / (float)n;
	tmp += new_sample / (float)n;
	return tmp;
}

void SpectreHandler::dataPostprocess() {
	for (int i = 0; i < spectreSize; i++) {
		float psd = this->psd(outData[i][0], outData[i][1]) + config->spectre.spectreCorrectionDb;
		if (firstRun) {
			tmpArray[i] = psd;
			firstRun = false;
		} else {
			tmpArray[i] = average(tmpArray[i], psd, config->spectre.spectreSpeed);
		}
	}

	//memcpy(tmpArray2, tmpArray, sizeof(float) * spectreSize);
	//superOutput[j] = (tmpArray[j - 1] + tmpArray[j] + tmpArray[j + 1]) / 3;
	//tmpArray2[j] = (tmpArray[j - 3] + 2 * tmpArray[j - 2] + 3 * tmpArray[j - 1] + 4 * tmpArray[j] + 3 * tmpArray[j + 1] + 2 * tmpArray[j + 2] + tmpArray[j + 3]) / 16.0;

	for (int n = 0; n <= config->spectre.smoothingDepth; n++) {
		for (int j = 0; j < spectreSize; j++) {
			j = Utils::convFFTResBinToSpecBin(j, spectreSize);
			if (j >= 1 && j < spectreSize - 1) {
				tmpArray2[j] = (tmpArray[j - 1] + tmpArray[j] + tmpArray[j + 1]) / 3;
				//tmpArray2[j] = (tmpArray[j - 2] + 2.0f * tmpArray[j - 1] + 3.0f * tmpArray[j] + 2.0f * tmpArray[j + 1] + tmpArray[j + 2]) / 9.0f;
			} else {
				tmpArray2[j] = tmpArray[j];
			}

			if (n == 0) {
				outputWaterfall[j] = tmpArray2[j];
			}

			if (n == config->spectre.smoothingDepth) {
				if (config->spectre.hangAndDecay) {
					if (superOutput[j] < tmpArray2[j]) {
						superOutput[j] = average(superOutput[j], tmpArray2[j], config->spectre.spectreSpeed2);
						speedDelta[j] = 1.0f;
					}
					else {
						superOutput[j] -= config->spectre.decaySpeed * speedDelta[j];
						speedDelta[j] += config->spectre.decaySpeedDelta;
					}
				}
				else {
					superOutput[j] = average(superOutput[j], tmpArray2[j], config->spectre.spectreSpeed2);
				}
			}
		}
		if (n != config->spectre.smoothingDepth) memcpy(tmpArray, tmpArray2, sizeof(float) * spectreSize);
	}
}

float SpectreHandler::psd(float re, float im) {
	//return (20*Math.log10(Math.sqrt((re*re)/binBandwidth + (im*im)/binBandwidth)));
	float basis = sqrt((re * re + im * im) / (float)config->fftLen);
	if (basis == 0) basis = 0.000001;
	return 10.0f * log(basis);
}

FFTData* SpectreHandler::getFFTData() {
	return fftData;
}

std::thread SpectreHandler::start() {
	std::thread p(&SpectreHandler::run, this);
	//SetThreadPriority(p.native_handle(), THREAD_PRIORITY_HIGHEST);
	//DWORD result = ::SetThreadIdealProcessor(p.native_handle(), 1);
	return p;
}


//std::atomic_int idx = 0;

/*void SpectreHandler::putData(float* data) {
	if (!spectreDataMutex.try_lock()) {
		return;
	}
	memcpy(dataBuffer, data, sizeof(float) * config->fftLen);
	spectreDataMutex.unlock(); // не забываем ставить unlock()!!!
	ready = true;
}*/

/*void SpectreHandler::write(float val) {
	if (ready) return;
	dataBuffer[idx] = val;
	idx++;
	if (idx >= spectreSize) ready = true;
}*/

/*void SpectreHandler::reset() {
	idx = 0;
}*/


/*float* FFTSpectreHandler::getOutputCopy(int startPos, int len, bool forWaterfall) {
	float* buffer = new float[spectreSize];

	//spectreDataMutex.lock();

	float* data = (forWaterfall == true) ? outputWaterfall : superOutput;

	memcpy(buffer, data + (spectreSize / 2), sizeof(data) * (spectreSize / 2));
	memcpy(buffer + (spectreSize / 2), data, sizeof(data) * (spectreSize / 2));

	//spectreDataMutex.unlock();

	float* dataCopy = new float[len];

	memcpy(dataCopy, buffer + startPos, sizeof(float) * len);

	delete[] buffer;

	return dataCopy;
}*/

//memcpy(output, dataBuffer, sizeof(output) * bufferLen);

//memset(imOut, 0, sizeof(float) * bufferLen);

//float* realIn = new float[bufferLen];
//memset(realIn, 0, sizeof(float) * bufferLen);

//prepareData();

//std::vector<float> re(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));
//std::vector<float> im(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));

//cout << "im.size() " << audiofft::AudioFFT::ComplexSize(FFT_LENGTH) << "\r\n";

//auto begin = std::chrono::steady_clock::now();

//fft3(realInput, imInput, config->fftLen / 2, realOut, imOut);

/*auto end = std::chrono::steady_clock::now();
auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
std::cout << "The time: " << elapsed_ms.count() << " micros\n";*/

//for (int i = 0; i < 32 * 1024; i++) printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);

/*for (int i = 0; i < config->fftLen / 2; i++) {
	realOut[i] = out[i][0];
	imOut[i] = out[i][1];
}*/

//fft3(realInput, imInput, config->fftLen / 2, realOut, imOut);

//fft.fft(dataBuffer, realOut, imOut);

//memcpy(output, realOut, sizeof(realOut));
//memcpy(output + sizeof(realOut), imOut, sizeof(imOut));


//Utils::printArray(inOut, bufferLen);

//fft.process(1, 10, output, kakashka);
//semaphore->unlock();