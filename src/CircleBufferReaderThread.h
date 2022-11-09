#pragma once

#include "CircleBuffer.h"
#include "SoundProcessorThread.h"

class CircleBufferReaderThread {

	Config* config;

	CircleBuffer* �ircleBuffer;

	SoundProcessorThread* soundProcessor;

	FFTSpectreHandler* fftSpectreHandler;

public: 

	CircleBufferReaderThread(Config* config, CircleBuffer* cb, FFTSpectreHandler* fftSpectreHandler, SoundProcessorThread* sP);

	void run();
	std::thread start();

};