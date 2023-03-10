#include "FFTSpectreHandler.h"

float* tmpArray;
float* tmpArray2;

FFTSpectreHandler::~FFTSpectreHandler() {
	fftw_destroy_plan(fftwPlan);
}

std::future<void> s;

FFTSpectreHandler::FFTSpectreHandler(Config* config) {
	this->config = config;

	spectreSize = config->fftLen / 2;

	wb = new WindowBlackman(config->fftLen);
	wbh = new WindowBlackmanHarris(config->fftLen);

	dataBuffer = new float[config->fftLen];
	memset(dataBuffer, 0, sizeof(float) * config->fftLen);

	complexLen = config->fftLen / 2 + 1;

	realInput = new float[spectreSize];
	//memset(realInput, 0, sizeof(float) * config->fftLen / 2);

	imInput = new float[spectreSize];
	//memset(imInput, 0, sizeof(float) * config->fftLen / 2);

	realOut = new float[complexLen];
	//memset(realOut, 0, sizeof(float) * complexLen);

	imOut = new float[complexLen];
	//memset(imOut, 0, sizeof(float) * complexLen);

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

	fftwPlan = fftw_plan_dft_1d(spectreSize, inData, outData, FFTW_FORWARD, FFTW_ESTIMATE);

	speedDelta = new float[spectreSize];
	//memset(speedDelta, 1, sizeof(float) * spectreSize);
}

std::mutex spectreDataMutex;

bool ready = false;

void FFTSpectreHandler::run() {
	while (true) {
		if (ready) {
			spectreDataMutex.lock();
			processFFT();
			spectreDataMutex.unlock();
			ready = false;
		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}

void FFTSpectreHandler::putData(float* data) {
	if (!spectreDataMutex.try_lock()) {
		return;
	}
	memcpy(dataBuffer, data, sizeof(float) * config->fftLen);
	spectreDataMutex.unlock(); // не забываем ставить unlock()!!!*/
	ready = true;
}

float* FFTSpectreHandler::getOutputCopy(int startPos, int len, bool forWaterfall) {
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
}

void FFTSpectreHandler::processFFT() {
	
	//memcpy(output, dataBuffer, sizeof(output) * bufferLen);

	//memset(imOut, 0, sizeof(float) * bufferLen);

	//float* realIn = new float[bufferLen];
	//memset(realIn, 0, sizeof(float) * bufferLen);

	//prepareData();

	//std::vector<float> re(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));
	//std::vector<float> im(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));

	//cout << "im.size() " << audiofft::AudioFFT::ComplexSize(FFT_LENGTH) << "\r\n";

	//Apply window function
	for (int i = 0; i < spectreSize; i++) {
		inData[i][0] = dataBuffer[2 * i] * wbh->getWeights()[i];
		inData[i][1] = dataBuffer[2 * i + 1] * wbh->getWeights()[i];
	}

	fftw_execute(fftwPlan);

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


	dataPostprocess();

	//Utils::printArray(inOut, bufferLen);

	//fft.process(1, 10, output, kakashka);
	//semaphore->unlock();
}


float FFTSpectreHandler::average(float avg, float new_sample, int n) {
	float tmp = avg;
	tmp -= avg / (float)n;
	tmp += new_sample / (float)n;
	return tmp;
}

void FFTSpectreHandler::dataPostprocess() {
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
			if (j >= 2 && j < spectreSize - 2) {
				//tmpArray2[j] = (tmpArray[j - 1] + tmpArray[j] + tmpArray[j + 1]) / 3;
				tmpArray2[j] = (tmpArray[j - 2] + 2.0f * tmpArray[j - 1] + 3.0f * tmpArray[j] + 2.0f * tmpArray[j + 1] + tmpArray[j + 2]) / 9.0f;
			}
			else {
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
				} else {
					superOutput[j] = average(superOutput[j], tmpArray2[j], config->spectre.spectreSpeed2);
				}
			}
		}
		if (n != config->spectre.smoothingDepth) memcpy(tmpArray, tmpArray2, sizeof(float) * spectreSize);
	}
}

int FFTSpectreHandler::getSpectreSize() {
	return spectreSize;
}

float FFTSpectreHandler::psd(float re, float im) {
	return 10 * log(re * re + im * im);
}

/*void FFTSpectreHandler::prepareData() {
	//Применения окна Блэкмона к исходным данным
	float* weights = wbh->getWeights();
	for (int i = 0; i < spectreSize; i++) {
		dataBuffer[2 * i] = dataBuffer[2 * i] * weights[i];
		dataBuffer[2 * i + 1] = dataBuffer[2 * i + 1] * weights[i];
	}
}*/

std::thread FFTSpectreHandler::start() {
	std::thread p(&FFTSpectreHandler::run, this);
	SetThreadPriority(p.native_handle(), THREAD_PRIORITY_HIGHEST);
	DWORD result = ::SetThreadIdealProcessor(p.native_handle(), 2);
	return p;
}