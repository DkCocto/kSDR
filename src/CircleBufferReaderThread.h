#pragma once

#include "CircleBuffer.h"
#include "SoundProcessorThread.h"

class CircleBufferReaderThread {

	Config* config;

	CircleBuffer* �ircleBuffer;

	SoundProcessorThread* soundProcessor;

public: 

	CircleBufferReaderThread(Config* config, CircleBuffer* cb, SoundProcessorThread* sP);

	void run();
	std::thread start();

};