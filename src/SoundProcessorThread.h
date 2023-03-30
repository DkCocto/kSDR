#pragma once

#include "Config.h"
#include "Mixer.h"
#include "HilbertTransform.h"
#include "Delay.h"
#include "PolyPhaseFilter.h"
#include "CircleBuffer.h"
#include "AGC.h"
#include "FIR.h"
#include "FMDemodulator.h"
#include "Device/DeviceController.h"
#include "ReceiverLogic.h"
#include "Thread/MyThread.h"


class SoundProcessorThread : public MyThread {

	//float* data = nullptr;
	//int dataLen = 0;

	float* outputData = nullptr;

	Mixer mixer;

	HilbertTransform hilbertTransform;
	Delay delay;

	PolyPhaseFilter firFilterI;
	PolyPhaseFilter firFilterQ;

	FirFilter audioFilter;

	double* decimateBufferI;
	double* decimateBufferQ;

	CircleBufferNew<float>* soundWriterCircleBuffer;
	SpectreHandler* specHandler;

	Config* config;

	FIR fir;
	FIR firI;
	FIR firQ;
	//FIR audioFilterFM;

	DeviceController* devCnt;

	ViewModel* viewModel;

	ReceiverLogic* receiverLogic;

	AGC agc;

	FMDemodulator fmDemodulator;

	DCRemove dcRemover;

	template<typename DEVICE, typename DATATYPE> void initProcess(DEVICE* device);

public: 

	int len; //Размер считывания за 1 раз из кругового буфера

	SoundProcessorThread(DeviceController* devCnt, ViewModel* viewModel, ReceiverLogic* receiverLogic, Config* config, CircleBufferNew<float>* soundWriterCircleBuffer, SpectreHandler* specHandler);
	~SoundProcessorThread();

	void initFilters(int filterWidth);

	template<typename T, typename D> void processData(T* data, D* device);

	void run();

	std::thread start();
};