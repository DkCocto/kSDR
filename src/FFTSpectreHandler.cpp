#include "FFTSpectreHandler.h"

#include "fft3.hpp"

FFTSpectreHandler::FFTSpectreHandler(Config* config) {
	this->config = config;

	wb = new WindowBlackman(config->fftLen);
	wbh = new WindowBlackmanHarris(config->fftLen);
	//windowArray = wb->init();

	dataBuffer = new float[config->fftLen];
	//memset(dataBuffer, 0, sizeof(float) * config->fftLen);

	complexLen = config->fftLen / 2 + 1;

	realInput = new float[config->fftLen / 2];
	//memset(realInput, 0, sizeof(float) * config->fftLen / 2);

	imInput = new float[config->fftLen / 2];
	//memset(imInput, 0, sizeof(float) * config->fftLen / 2);

	realOut = new float[complexLen];
	//memset(realOut, 0, sizeof(float) * complexLen);

	imOut = new float[complexLen];
	//memset(imOut, 0, sizeof(float) * complexLen);

	superOutput = new float[config->fftLen / 2];
	memset(superOutput, 0, sizeof(float) * config->fftLen / 2);

	spectreSize = config->fftLen / 2;
}

std::queue<std::vector<float>> spectreDataQueue;
std::mutex spectreDataMutex;

bool readyToCalculate = false;

void FFTSpectreHandler::run() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		if (readyToCalculate) {
			if (!spectreDataMutex.try_lock()) continue;
			processFFT();
			readyToCalculate = false;
			 // не забываем ставить unlock()!!!
			spectreDataMutex.unlock();
		}
	}
}

//Возвращает объект очереди и ставит lock для потока
std::queue<std::vector<float>> FFTSpectreHandler::getSpectreDataQueue() {
	return spectreDataQueue;
}

	
void FFTSpectreHandler::putData(float* data) {
	if (!spectreDataMutex.try_lock()) return;

	memcpy(dataBuffer, data, sizeof(data) * config->fftLen);

	spectreDataMutex.unlock(); // не забываем ставить unlock()!!!
	readyToCalculate = true;
}

float* FFTSpectreHandler::getOutputCopy(int startPos, int len) {
	//memset(dataCopy, 0.001, sizeof(float) * len);

	//spectreDataMutex.lock();
	float* output = getOutput();
	//float* output = &myQueueDisplay.front()[0];

	int spectreSize = getSpectreSize();

	float* buffer = new float[spectreSize];
	memcpy(buffer, output + (spectreSize / 2), sizeof(output) * (spectreSize / 2));
	memcpy(buffer + (spectreSize / 2), output, sizeof(output) * (spectreSize / 2));

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

	prepareData();

	//std::vector<float> re(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));
	//std::vector<float> im(audiofft::AudioFFT::ComplexSize(FFT_LENGTH));

	//cout << "im.size() " << audiofft::AudioFFT::ComplexSize(FFT_LENGTH) << "\r\n";

	for (int i = 0; i < config->fftLen / 2; i++) {
		realInput[i] = dataBuffer[2 * i];
		imInput[i] = dataBuffer[2 * i + 1];
	}

	fft3(realInput, imInput, config->fftLen / 2, realOut, imOut);


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
	for (int i = 0; i < config->fftLen / 2; i++) {
		float psd = this->psd(realOut[i], imOut[i]);
		if (firstRun) {
			superOutput[i] = psd;
			firstRun = false;
		}
		else {
			superOutput[i] = average(superOutput[i], psd, spectreSpeed);
		}
	}
}

Semaphore* FFTSpectreHandler::getSemaphore() {
	return semaphore;
}


float* FFTSpectreHandler::getOutput() {
	return superOutput;
}

int FFTSpectreHandler::getSpectreSize() {
	return spectreSize;
}

float FFTSpectreHandler::psd(float re, float im) {
	return 10 * log((sqrt(re * re + im * im)));
}

void FFTSpectreHandler::prepareData() {
	//Применения окна Блэкмона к исходным данным
	float* weights = wb->getWeights();
	for (int i = 0; i < config->fftLen / 2; i++) {
		dataBuffer[2 * i] = dataBuffer[2 * i] * weights[i];
		dataBuffer[2 * i + 1] = dataBuffer[2 * i + 1] * weights[i];
	}
}

/*int FFTSpectreHandler::getTrueBin(int bin) {
	//bin[0, 1022]

	int size = (complexLen * 2 - 1) / 2; // 1025

	int increment = 0;
	if (bin >= (size - 1) / 2) { //bin >= 512
		increment = -1 * ((size - 1) / 2) + 1; //inc = -512
	}
	else {
		increment = (size - 1) / 2 + 1; // inc = 513 
	}
	//printf("%i ", bin + increment);


	//0 -> 1026; 1024 -> -1!!!; 1023 -> 2049; 1022 -> 
	// 2049 -> 2049 - 1026 = 1023

	return bin + increment;
}*/

void FFTSpectreHandler::setSpectreSpeed(int speed) {
	spectreSpeed = speed;
}

std::thread FFTSpectreHandler::start() {
	std::thread p(&FFTSpectreHandler::run, this);
	return p;
}