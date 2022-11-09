#pragma once

#include "CircleBuffer.h"
#include "SoundCard.h"

class CircleBufferWriterThread {

	CircleBuffer* soundWriterCircleBuffer;
	SoundCard* soundCard;

	int len;

	Config* config;

public:

	CircleBufferWriterThread(Config* config, CircleBuffer* cb, SoundCard* sc);

	void run();
	std::thread start();

};

