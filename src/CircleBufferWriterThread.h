#pragma once

#include "CircleBuffer.h"
#include "SoundCard.h"
#include "Device/DeviceController.h"
#include "windows.h"
#include "Thread/MyThread.h"

class CircleBufferWriterThread : public MyThread {

	CircleBuffer* soundWriterCircleBuffer;
	SoundCard* soundCard;

	int len;

	Config* config;
	DeviceController* deviceController;

public:

	CircleBufferWriterThread(Config* config, DeviceController* deviceController, CircleBuffer* cb, SoundCard* sc);

	void run();
	std::thread start();
};

