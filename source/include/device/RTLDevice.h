#pragma once

#include "DeviceN.h"
#include "CircleBufferNew.h"
#include <vector>
#include "rtl-sdr.h"

//#define FREQ_POPRAVKA			52434810

class RTLDevice : public DeviceN {
	private:
		
		CircleBufferNew<unsigned char>* bufferForSpec = nullptr;
		CircleBufferNew<unsigned char>* bufferForProc = nullptr;

		const bool DEBUG = true;

		Result initResult;

	public:

		rtlsdr_dev_t* device = nullptr;
		std::atomic_bool processing = false;

		RTLDevice(Config* config) : DeviceN(config) {
			bufferForSpec = new CircleBufferNew<unsigned char>(config);
			bufferForProc = new CircleBufferNew<unsigned char>(config);
		};

		~RTLDevice();

		Result start();

		std::vector<uint32_t>* listDevices();
		int openDevice(uint32_t deviceIndex);

		int setFreq(uint32_t frequency);
		int setSampleRate(uint32_t sampRate);
		int enableAutoGain(bool enable);
		int resetBuffer();
		static void rtlsdr_callback(unsigned char* buf, uint32_t len, void* ctx);
		int startAsync();
		int closeDevice();
		int nearest_gain(int targetGain);


		int setGain(int gain);
		void stop();

		CircleBufferNew<unsigned char>* getBufferForSpec();
		CircleBufferNew<unsigned char>* getBufferForProc();

		float prepareData(unsigned char val);

};