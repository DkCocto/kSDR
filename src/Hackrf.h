#pragma once

#include "hackrf\hackrf.h"
#include "Config.h"
#include "CircleBuffer.h"
#include "ViewModel.h"


class Hackrf {

	Config* config;
	CircleBuffer* cb;

	double savedFreq;
	unsigned int savedLnaGain, savedVgaGain;
	bool savedAmp;

public:

	Hackrf(Config* config, CircleBuffer* cb);
	~Hackrf();

	void close();

	static int rx_callback(hackrf_transfer* transfer);

	void init();

	void stopRX();

	hackrf_error startRX();

	void setFreq(uint64_t freq);

	void setLnaGain(unsigned int gain);

	void setVgaGain(unsigned int gain);

	void enableAmp(bool enabled);

};