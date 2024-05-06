#pragma once

#include "Thread/MyThread.h"
#include "CircleBufferNew.h"
#include "SoundCard.h"
#include "windows.h"
#include "FIR.h"

//Class reads data from sound input, filter it and resample. After that stores it to the buffer.
class SoundCardInputReaderThread : public MyThread {

private:

	bool reading = false;

	CircleBufferNew<float>* soundInputBuffer; //stores data here
	SoundCard* soundCard;
	Config* config;

	FIR audioFilter, resamplingFilter;

	void run();

	enum Status {
		START_READING,
		PAUSE,
		REST,
		READING
	} CurrentStatus = REST;

public:

	SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* sc);
	~SoundCardInputReaderThread();

	std::thread start();
	void pause();
	void continueRead();
};