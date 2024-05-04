#pragma once

#include "CircleBuffer.h"
#include "SoundCard.h"
#include "Device/DeviceController.h"
#include "windows.h"
#include "Thread/MyThread.h"

class SoundCardWriterThread : public MyThread {

	CircleBufferNew<float>* soundWriterCircleBuffer;
	SoundCard* soundCard;

	int len;

	Config* config;
	DeviceController* deviceController;

	float* data;

public:

	SoundCardWriterThread(Config* config, DeviceController* deviceController, CircleBufferNew<float>* cb, SoundCard* sc);
	~SoundCardWriterThread();
	void run();
	std::thread start();
};