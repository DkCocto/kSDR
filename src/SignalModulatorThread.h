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

	int const TONE_SIGNAL_FREQ = 1000;

	SinOscillator so;

	SSBModulation* ssbModulation;
	AMModulation* amModulation;

public:

	SignalModulatorThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* sc);
	~SignalModulatorThread();

	std::thread start();
};