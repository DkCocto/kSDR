#pragma once

#include "Env.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "rtlsdr/rtl-sdr.h"
#include "vector"
#include "CircleBuffer.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_BUF_LENGTH		(16 * 16384)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

class RTLDevice {

private:

	uint32_t sampRate = DEFAULT_SAMPLE_RATE;

	rtlsdr_dev_t* device;

	int nearest_gain(int targetGain);

public:

	std::vector<uint32_t>* listDevices();
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
	int startAsync(CircleBuffer* circleBuffer);

};