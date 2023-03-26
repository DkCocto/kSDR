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

	CircleBuffer* iqSignalsCircleBuffer;
	CircleBuffer* soundWriterCircleBuffer;
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

public: 

	int len; //Размер считывания за 1 раз из кругового буфера

	SoundProcessorThread(DeviceController* devCnt, ViewModel* viewModel, ReceiverLogic* receiverLogic, Config* config, CircleBuffer* sPCB, CircleBuffer* sWCB, SpectreHandler* specHandler);
	~SoundProcessorThread();

	void initFilters(int filterWidth);

	void process();

	std::thread start();

};