#pragma once

#include "Thread/MyThread.h"
#include "CircleBufferNew.h"
#include "SoundCard.h"
#include "FIR.h"
#include "FastFir/jfastfir.h"
#include "SinOscillator.h"

//Class reads data from sound input, filter it and resample. After that stores it to the buffer.
class SoundCardInputReaderThread : public MyThread {

private:

	//create pointer
	JFastFIRFilter* fastfir = nullptr;

	bool reading = false;

	CircleBufferNew<float>* soundInputBuffer; //stores data here
	SoundCard* soundCard;
	Config* config;

	int audioFilterWidth = 0;
	int resamplingFilterWidth = 0;

	FIR audioFilter;

	void run();

	void initFilters(int audioFilterWidth);

	int const TONE_SIGNAL_FREQ = 1000;

public:

	SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* sc);
	~SoundCardInputReaderThread();

	std::thread start();
};