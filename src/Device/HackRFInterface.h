#pragma once

#include "stdint.h"
#include "HackRFDevice.h"

class HackRfInterface {

	uint64_t savedFreq = 7100000;
	bool needToSetFreq = false;

	uint32_t savedBaseband = 1750000;
	bool needToSetBaseband = false;

	uint32_t savedLnaGain = -1;
	bool needToSetLnaGain = false;

	uint32_t savedVgaGain = -1;
	bool needToSetVgaGain = false;

	uint8_t savedAmp = 0;
	bool needToSetAmp = false;

	HackRFDevice* hackRFDevice;

public:

	HackRfInterface(HackRFDevice* hackRFDevice);

	void setFreq(uint64_t freq);
	void setLnaGain(uint32_t gain);
	void setVgaGain(uint32_t gain);
	void setBaseband(int baseband);
	void enableAmp(uint8_t amp);

	void sendParamsToDevice();
};