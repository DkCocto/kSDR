#pragma once

#include "CircleBuffer.h"
#include "SoundCard.h"
#include "Device/DeviceController.h"
#include "windows.h"

class CircleBufferWriterThread {

	CircleBuffer* soundWriterCircleBuffer;
	SoundCard* soundCard;

	int len;

	Config* config;
	DeviceController* deviceController;

	bool isWorking_ = false;

public:

	CircleBufferWriterThread(Config* config, DeviceController* deviceController, CircleBuffer* cb, SoundCard* sc);

	void run();
	std::thread start();
	bool isWorking();
};

