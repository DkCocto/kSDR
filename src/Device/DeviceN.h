#pragma once

#include "../CircleBuffer.h"

class DeviceN {

	DeviceN();

	enum DeviceType {
		RSP,
		HACKRF,
		RTL
	} deviceType;

	virtual void setFreq(int frequency);
	virtual void setSampleRate(int sampleRate);
	virtual void init();
	virtual void start();
	virtual void stop();
	virtual void addReceiver(CircleBuffer* circleBuffer);

};