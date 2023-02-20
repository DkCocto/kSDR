#pragma once

#include "hackrf\hackrf.h"
#include "Config.h"
#include "CircleBuffer.h"
#include "ViewModel.h"
#include "Device.h"
#include "Display.h"


class Hackrf: public Device {

	Config* config;
	CircleBuffer* cb;

	uint64_t savedFreq;
	uint32_t savedBaseband = 1750000;

	uint32_t savedLnaGain = -1;
	uint32_t savedVgaGain = -1;
	uint8_t savedAmp = 0;

	ViewModel* viewModel;

public:

	Hackrf(Config* config, CircleBuffer* cb);
	~Hackrf();

	void close();

	static int rx_callback(hackrf_transfer* transfer);

	void init();

	void stopRX();

	hackrf_error startRX();

	void setFreq(uint64_t freq);

	void setLnaGain(uint32_t gain);

	void setVgaGain(uint32_t gain);
	void setBaseband(uint32_t baseband);

	int parse_u32(char* s, uint32_t* const value);

	void enableAmp(uint8_t amp);

	bool isNeedToSetupFreq();
	bool isNeedToSetupLnaGain();
	bool isNeedToSetupVgnGain();
	bool isNeedToSetupAmp();
	bool isNeedToSetupFilter();

};