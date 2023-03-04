#pragma once

#include "Env.h"
#include "Config.h"
#include "Semaphore.h"
#include "WindowBlackman.h"
#include "WindowBlackmanHarris.h"
#include "Average.h"
#include "vector"
#include "queue"
#include "mutex"
#include "KalmanFilter.h"
#include "fftw/fftw3.h"
#include "iostream"

//#include "AudioFFT.h"

class FFTSpectreHandler {
	
private:

	//audiofft::AudioFFT fft;

	Config* config;
	
	//»нициализируем оконный массив с размерностью длины массива буфера спектра (возвращаетс¤ массив длиной +1)
	WindowBlackman* wb;
	WindowBlackmanHarris* wbh;

	float* dataBuffer;

	int savedBufferPos = -1;

	int complexLen;

	float* realInput;
	float* imInput;

	float* realOut;
	float* imOut;

	//float output[complexLen * 2];
	
	float* superOutput;
	float* outputWaterfall;

	//Semaphore* semaphore = new Semaphore();

	bool firstRun = true;

	int spectreSpeed = 30;

	//Average average1 = Average(20);

	void processFFT();
	float average(float avg, float new_sample, int n);
	void dataPostprocess();

	int spectreSize = 0;

	bool busy = false;
	bool readyToProcess = false;
	bool outputting = false;

	float psd(float re, float im);
	void prepareData();

	//std::queue<std::vector<float>> getSpectreDataQueue();

	fftw_plan fftwPlan;
	fftw_complex* inData;
	fftw_complex* outData;

	float* speedDelta;

public:

	FFTSpectreHandler(Config* config);
	~FFTSpectreHandler();
	float* getOutputCopy(int startPos, int len, bool forWaterfall);
	void putData(float* data);

	int getSpectreSize();
	//void setSpectreSpeed(int speed);

	std::thread start();
	void run();


};