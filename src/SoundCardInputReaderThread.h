#pragma once

#include "Thread/MyThread.h"
#include "CircleBufferNew.h"
#include "SoundCard.h"
#include "windows.h"
#include "FIR.h"

class SoundCardInputReaderThread : public MyThread {

private:

	CircleBufferNew<float>* soundInputBuffer;
	SoundCard* soundCard;
	Config* config;

	void run();

	FIR firFilter, firFilter2;

public:

	SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* sc);
	~SoundCardInputReaderThread();

	std::thread start();
};