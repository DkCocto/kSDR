#pragma once 

#include "RTLDevice.h"
#include "DeviceInterface.h"

class RTLInterface : public DeviceInterface {

	uint32_t savedFreq = 7100000;
	bool needToSetFreq = false;

	//uint32_t savedBaseband = 1750000;
	//bool needToSetBaseband = false;

	int savedGain = 40;
	bool needToSetGain = false;

public:

	RTLInterface(RTLDevice* rtlDevice) : DeviceInterface(rtlDevice) {};

	void setFreq(uint32_t freq);
	void setGain(int gain);
	void sendParamsToDevice();
};