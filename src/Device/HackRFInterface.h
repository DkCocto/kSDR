#pragma once

#include "stdint.h"
#include "HackRFDevice.h"
#include "DeviceInterface.h"
#include "../TransmittingData.h"

class HackRfInterface : public DeviceInterface {

	uint64_t savedFreq = 7100000;
	bool needToSetFreq = false;

	uint32_t savedBaseband = 1750000;
	bool needToSetBaseband = false;

	uint32_t savedLnaGain = -1;
	bool needToSetLnaGain = false;

	uint32_t savedVgaGain = -1;
	bool needToSetVgaGain = false;

	uint32_t savedTxVgaGain = -1;
	bool needToSetTxVgaGain = false;

	uint8_t savedAmp = 0;
	bool needToSetAmp = false;

	TransmittingData* transmittingData;

public:

	HackRfInterface(HackRFDevice* hackRFDevice) : DeviceInterface(hackRFDevice) {
		transmittingData = new TransmittingData(hackRFDevice->config, 0, hackRFDevice->config->hackrf.deviceSamplingRate, HACKRF_TX_BUFFER_LEN);
		((HackRFDevice*)device)->setDataForTransmitting(transmittingData);
	};

	~HackRfInterface();

	void setTXBuffer(CircleBufferNew<float>* txBuffer);
	CircleBufferNew<float>* getTXBuffer();

	void setFreq(uint64_t freq);
	void setLnaGain(uint32_t gain);
	void setVgaGain(uint32_t gain);
	void setTxVgaGain(uint32_t gain);
	void setBaseband(int baseband);
	void enableAmp(uint8_t amp);

	bool isDeviceTransmitting();

	bool pauseRX();
	bool releasePauseRX();
	bool startTX(int freq);
	bool stopTX();

	void sendParamsToDevice();
};