#pragma once

#include "hackrf\hackrf.h"
#include "DeviceN.h"
#include "../CircleBufferNew.h"
#include "../TransmittingData.h"

#define HACKRF_TX_BUFFER_LEN 262144
#define HACKRF_TX_BUFFER_HALF_LEN 131072

class HackRFDevice : public DeviceN {
	private:

		const bool DEBUG = true;

		hackrf_device* device = NULL;

		static int rx_callback(hackrf_transfer* transfer);
		static int tx_callback(hackrf_transfer* transfer);

		uint8_t chuchka(uint8_t val);

		CircleBufferNew<uint8_t>* bufferForSpec;
		CircleBufferNew<uint8_t>* bufferForProc;

		TransmittingData* transmittingData = nullptr;

		bool isTxOn = false;

	public:
		void setFreq(uint64_t frequency);
		void setLnaGain(uint32_t gain);
		void setVgaGain(uint32_t gain);
		void setTxVgaGain(uint32_t gain);
		void setBaseband(int baseband);
		void enableAmp(uint8_t amp);
		void setDataForTransmitting(TransmittingData* transmittingData);

		bool isDeviceTransmitting();

		HackRFDevice(Config* config) : DeviceN(config) {
			bufferForSpec = new CircleBufferNew<uint8_t>(config);
			bufferForProc = new CircleBufferNew<uint8_t>(config);
		};

		~HackRFDevice();

		Result start();
		bool startTX();
		bool stopTX();
		bool pauseRX();
		bool releasePauseRX();
		void stop();

		CircleBufferNew<uint8_t>* getBufferForSpec();
		CircleBufferNew<uint8_t>* getBufferForProc();

		float prepareData(uint8_t val);
};