#pragma once

#include "Env.h"
#include "thread"
#include "vector"
#include "RTLDevice.h"
#include "CircleBuffer.h"
#include "Display.h"

class RTLDeviceReaderThread {

	CircleBuffer* cb;
	RTLDevice rtl;

	int savedFreq = 7100000;
	int savedGain = 0;

public:

	RTLDeviceReaderThread(CircleBuffer* cb);
	~RTLDeviceReaderThread();

	void process();

	std::thread start();

	void initRTLDevice();

};

