#pragma once

#include "Thread/MyThread.h"
#include "CircleBufferNew.h"
#include "FIR.h"
#include "SSB.h"

#define BUFFER_READ_LEN 8

//Class reads prepared sound card input buffer, makes modulation and put the data to the output buffer
class TXDataHandler : MyThread {

private:

	bool processing = true;

	Config* config;

	CircleBufferNew<float>* inputBuffer = nullptr;
	CircleBufferNew<float>* outputBuffer = nullptr;

	SSB ssb;

	void run();

public:
	TXDataHandler(Config* config, CircleBufferNew<float>* inputBuffer, CircleBufferNew<float>* outputBuffer);
	~TXDataHandler();

	std::thread start();
	void pause();
	void continueProcess();

};