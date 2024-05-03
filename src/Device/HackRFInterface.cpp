#include "HackRFInterface.h"

HackRfInterface::~HackRfInterface() {
	delete transmittingData;
}

void HackRfInterface::setFreq(uint64_t freq) {
	if (savedFreq != freq) {
		savedFreq = freq;
		needToSetFreq = true;
	}
}

void HackRfInterface::setLnaGain(uint32_t gain) {
	if (savedLnaGain != gain) {
		savedLnaGain = gain;
		needToSetLnaGain = true;
	}
}

void HackRfInterface::setVgaGain(uint32_t gain) {
	if (savedVgaGain != gain) {
		savedVgaGain = gain;
		needToSetVgaGain = true;
	}
}

void HackRfInterface::setTxVgaGain(uint32_t gain) {
	if (savedTxVgaGain != gain) {
		savedTxVgaGain = gain;
		needToSetTxVgaGain = true;
	}
}

void HackRfInterface::setBaseband(int baseband) {
	if (savedBaseband != baseband) {
		savedBaseband = baseband;
		needToSetBaseband = true;
	}
}

void HackRfInterface::enableAmp(uint8_t amp) {
	if (savedAmp != amp) {
		savedAmp = amp;
		needToSetAmp = true;
	}
}

bool HackRfInterface::isDeviceTransmitting() {
	return ((HackRFDevice*)device)->isDeviceTransmitting();
}

bool HackRfInterface::pauseRX() {
	return ((HackRFDevice*)device)->pauseRX();
}

bool HackRfInterface::releasePauseRX() {
	return ((HackRFDevice*)device)->releasePauseRX();
}

bool HackRfInterface::startTX(int freq) {
	transmittingData->setFreq(freq);
	return ((HackRFDevice*)device)->startTX();
}

bool HackRfInterface::stopTX() {
	return ((HackRFDevice*)device)->stopTX();
}

void HackRfInterface::sendParamsToDevice() {

	HackRFDevice* hackRFDevice = (HackRFDevice*)device;

	if (needToSetFreq) {
		hackRFDevice->setFreq((savedFreq + ((hackRFDevice->config->receiver.enableFrequencyShift == true) ? hackRFDevice->config->receiver.frequencyShift : 0)));
		needToSetFreq = false;
	}

	if (needToSetLnaGain) {
		hackRFDevice->setLnaGain(savedLnaGain * 8);
		needToSetLnaGain = false;
	}

	if (needToSetVgaGain) {
		hackRFDevice->setVgaGain(savedVgaGain * 2);
		needToSetVgaGain = false;
	}

	if (needToSetTxVgaGain) {
		hackRFDevice->setTxVgaGain(savedTxVgaGain);
		needToSetTxVgaGain = false;
	}

	if (needToSetBaseband) {
		hackRFDevice->setBaseband(savedBaseband);
		needToSetBaseband = false;
	}

	if (needToSetAmp) {
		hackRFDevice->enableAmp(savedAmp);
		needToSetAmp = false;
	}
}