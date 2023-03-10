#pragma once

#include "Config.h"
#include "Mixer.h"
#include "HilbertTransform.h"
#include "Delay.h"
#include "PolyPhaseFilter.h"
#include "SoundCard.h"
#include "CircleBuffer.h"
#include "AGC.h"
#include "ReceiverLogic.h"
#include "Display.h"
#include "FIR.h"
#include "DCRemove.h"
#include "FMDemodulator.h"
#include "Windows.h"
#include "SpeedCount.h"

using namespace std;

class SoundProcessorThread {

	//float* data = nullptr;
	//int dataLen = 0;

	float* outputData = nullptr;

	Mixer* mixer;

	HilbertTransform* hilbertTransform;
	Delay* delay;

	PolyPhaseFilter firFilterI;
	PolyPhaseFilter firFilterQ;

	FirFilter* audioFilter;

	double* decimateBufferI;
	double* decimateBufferQ;

	CircleBuffer* iqSignalsCircleBuffer;
	CircleBuffer* soundWriterCircleBuffer;
	FFTSpectreHandler* fftSpectreHandler;

	Config* config;

	FIR* audioFir = new FIR();
	FIR* firI = new FIR();
	FIR* firQ = new FIR();

	FIR* prefirI = new FIR();
	FIR* prefirQ = new FIR();
	//FIR audioFilterFM;

	DCRemove dcRemove;

public: 

	AGC* agc;

	int len; //Размер считывания за 1 раз из кругового буфера

	SoundProcessorThread(Config* config, CircleBuffer* sPCB, CircleBuffer* sWCB, FFTSpectreHandler* fftSpectreHandler);

	void initFilters(int filterWidth);

	void process();

	std::thread start();
};