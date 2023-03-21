#pragma once

#include "hackrf\hackrf.h"
#include "DeviceN.h"
#include "string"

constexpr bool DEBUG = true;

class HackRFDevice : public DeviceN {
	private:

		hackrf_device* device = NULL;

		static int rx_callback(hackrf_transfer* transfer);

	public:

		void setFreq(uint64_t frequency);
		void setLnaGain(uint32_t gain);
		void setVgaGain(uint32_t gain);
		void setBaseband(int baseband);
		void enableAmp(uint8_t amp);

		HackRFDevice(Config* config) : DeviceN(config) { };

		~HackRFDevice();

		void setSampleRate(int sampleRate);

		Result start();
		void stop();
};