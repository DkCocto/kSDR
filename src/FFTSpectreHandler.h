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
#include "DataReceiver.h"
#include "DCRemove.h"
#include "Device/DeviceController.h"

class SpectreHandler : public MyThread {
	
private:

	float* veryRawDataArray;
	float* spectreRawArray;
	float* spectreRaw2Array;
	float* tmpArray2;

	//audiofft::AudioFFT fft;

	Config* config;
	
	//»нициализируем оконный массив с размерностью длины массива буфера спектра (возвращаетс¤ массив длиной +1)
	WindowBlackman* wb;
	WindowBlackmanHarris* wbh;

	int savedBufferPos = -1;
	
	float* superOutput;
	float* outputWaterfall;

	bool firstRun = true;

	int spectreSpeed = 30;

	int spectreSize = 0;

	bool busy = false;
	bool readyToProcess = false;
	bool outputting = false;

	fftw_plan fftwPlan;
	fftw_complex* inData;
	fftw_complex* outData;

	float* speedDelta;

	FFTData* fftData;

	std::mutex spectreDataMutex;

	std::atomic_bool ready = false;

	DCRemove dcRemove;

	ViewModel* viewModel;

	DeviceController* deviceController;

	void run();
	void dataPostprocess();
	template<typename DEVICE, typename DATATYPE> void prepareToProcess(DEVICE* device);
	template<typename T, typename D> void processFFT(T* data, D* device);
	float psd(float re, float im);
	float average(float avg, float new_sample, int n);

	void calculateAndNormalizeBinInRawData(float* data, int size);

	void prepareDataForWaterfall(float* data, int size);

public:

	SpectreHandler(Config* config, FFTData* fftData, ViewModel* viewModel, DeviceController* deviceController);
	~SpectreHandler();

	std::thread start();

	FFTData* getFFTData();
};