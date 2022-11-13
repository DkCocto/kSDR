#include "FFTSpectreHandler.h"

#include "fft3.hpp"

FFTSpectreHandler::FFTSpectreHandler(Config* config) {
	this->config = config;

	wb = new WindowBlackman(config->fftLen);
	windowArray = wb->init();

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
}

void FFTSpectreHandler::processFFT() {
	if (!semaphore->lock()) return;

	//memcpy(output, dataBuffer, sizeof(output) * bufferLen);

	//memset(imOut, 0, sizeof(float) * bufferLen);

	/*float* realIn = new float[bufferLen];
	memset(realIn, 0, sizeof(float) * bufferLen);*/

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


	//outputVector.clear();
	////outputVector.insert(outputVector.begin(), begin(output), end(output));
	//for (int i = 0; i < complexLen; i++) {
	//	outputVector.push_back(realOut[i]);
	//}
	//for (int i = 0; i < complexLen; i++) {
	//	outputVector.push_back(imOut[i]);
	//}


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
	//printf("EBLO %i \r\n", outputVector.size());
	for (int i = 0; i < config->fftLen / 2; i++) {
		float psd = this->psd(realOut[i], imOut[i]);
		if (firstRun) {
			superOutput[i] = psd;
			firstRun = false;
		}
		else {
			//superOutput[i] = average.process(psd);
			superOutput[i] = average(superOutput[i], psd, 20);
			//outputVector.at(i) = average.process(psd);
		}
	}
}

Semaphore* FFTSpectreHandler::getSemaphore() {
	return semaphore;
}


float* FFTSpectreHandler::getOutput() {
	return superOutput;
}

bool FFTSpectreHandler::putData(float* pieceOfData, int len) {

	int savedBufferLen = savedBufferPos + 1;

	if (savedBufferLen + len <= config->fftLen) {
		for (int i = 0; i < len; i++) {
			dataBuffer[savedBufferPos + 1 + i] = pieceOfData[i];
		}
	}
	else {
		for (int i = 0; i < config->fftLen - savedBufferLen; i++) {
			dataBuffer[savedBufferPos + 1 + i] = pieceOfData[i];
		}
	}

	if (savedBufferLen + len < config->fftLen) savedBufferPos += len;
	if (savedBufferLen + len >= config->fftLen) {
		savedBufferPos = -1;
		processFFT();
		return true;
	}

	return false;
}

float FFTSpectreHandler::psd(float re, float im) {
	return log((sqrt(re * re + im * im) / config->fftBandwidth)) / 13;
}

void FFTSpectreHandler::prepareData() {
	//Применения окна Блэкмона к исходным данным
	for (int i = 0; i < config->fftLen / 2; i++) {
		dataBuffer[2 * i] = dataBuffer[2 * i] * windowArray[i];
		dataBuffer[2 * i + 1] = dataBuffer[2 * i + 1] * windowArray[i];
	}
}


int FFTSpectreHandler::getTrueBin(int bin) {
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
}