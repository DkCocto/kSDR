#pragma once

#include "DeviceInterface.h"
#include "RSPDevice.h"

class RSPInterface : public DeviceInterface {

private:
	double savedFreq = 7100000;
	bool needToSetFreq = false;

	int savedGain = 0;
	unsigned char savedLnaState = 0;
	bool needToSetGain = false;

	int savedBasebandFilter = 600;
	bool needToSetFilter = false;

public:

	RSPInterface(RSPDevice* rspDevice) : DeviceInterface(rspDevice) {};

	void setFreq(double freq);
	void setGain(int gain, unsigned char lnaState);
	void setBasebandFilter(int filterWidth);

	void sendParamsToDevice();
};