#pragma once

#include "Env.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "rtlsdr/rtl-sdr.h"
#include "vector"
#include "CircleBuffer.h"
#include "Device.h"
#include "Config.h"
#include "Display.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_BUF_LENGTH		(16 * 16384)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

#define RTL_READ_BUFFER			512

//#define FREQ_POPRAVKA			52434810

using namespace std;

class RTLDevice: public Device {

private:

	uint32_t sampRate = DEFAULT_SAMPLE_RATE;

	rtlsdr_dev_t* device;

	int nearest_gain(int targetGain);

	Config* config;
	CircleBuffer* cb;

	ViewModel* viewModel;

	bool DEBUG = false;

	bool isNeedToSetFreq();
	int savedFreq;

	bool isNeedToSetGain();
	int savedGain;

public:

	RTLDevice(Config* config, CircleBuffer* cb);
	~RTLDevice();

	bool init();

	vector<uint32_t>* listDevices();
	int openDevice(uint32_t deviceIndex);
	rtlsdr_dev_t* getDevice();
	int setSampleRate(uint32_t sampRate);
	int setFrequency(uint32_t frequency);
	int enableAutoGain();
	int setGain(int gain);
	int resetBuffer();
	int readSync(uint8_t* buffer, uint32_t blockSize, int* n_read);
	int closeDevice();
	static void rtlsdr_callback(unsigned char* buf, uint32_t len, void* ctx);
	int startAsync();

	void setConfiguration();
};