#pragma once

#include "Env.h"
#include "WindowBlackman.h"
#include "WindowBlackmanHarris.h"
#include "mutex"
#include "fftw/fftw3.h"
#include "ViewModel.h"
#include "Thread/MyThread.h"
#include "Spectre/FFTData.h"
#include "windows.h"
#include "../DataReceiver.h"
#include "DCRemove.h"
#include "CircleBuffer.h"

class SpectreHandler : public MyThread {
	
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
	
	float* superOutput;
	float* outputWaterfall;

	bool firstRun = true;

	int spectreSpeed = 30;

	void processFFT();
	float average(float avg, float new_sample, int n);
	void dataPostprocess();

	int spectreSize = 0;

	bool busy = false;
	bool readyToProcess = false;
	bool outputting = false;

	float psd(float re, float im);

	fftw_plan fftwPlan;
	fftw_complex* inData;
	fftw_complex* outData;

	float* speedDelta;

	FFTData* fftData;

	std::mutex spectreDataMutex;

	std::atomic_bool ready = false;

	DCRemove dcRemove;

	ViewModel* viewModel;

	CircleBuffer* circleBuffer;

public:

	FFTData* getFFTData();

	SpectreHandler(Config* config, FFTData* fftData, ViewModel* viewModel, CircleBuffer* circleBuffer);
	~SpectreHandler();

	void putData(float* data);
	std::thread start();
	void run();
};