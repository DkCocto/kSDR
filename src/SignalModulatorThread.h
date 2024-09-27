#pragma once

#include "Thread/MyThread.h"
#include "CircleBufferNew.h"
#include "SoundCard.h"
#include "FIR.h"
#include "FastFir/jfastfir.h"
#include "CosOscillator.h"
#include "Delay.h"
#include "HilbertTransform.h"
#include "SSB.h"
#include "AM.h"

//Class reads data from sound input, modulate it and resample. After that stores the data to the circle buffer.
class SignalModulatorThread : public MyThread {

private:

	int BUFFER_READ_LEN;

	bool reading = false;

	CircleBufferNew<float>* outputSignalBuffer; //stores data here
	SoundCard* soundCard;
	Config* config;

	void run();

	int TONE_SIGNAL_FREQ1 = 0;
	int TONE_SIGNAL_FREQ2 = 0;

	SinOscillator so1, so2;

	SSBModulation* ssbModulation;
	AMModulation* amModulation;

	void initToneGenerator();

public:

	SignalModulatorThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* sc);
	~SignalModulatorThread();

	std::thread start();
};