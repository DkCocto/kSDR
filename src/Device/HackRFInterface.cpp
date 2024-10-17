#include "HackRFInterface.h"

HackRfInterface::~HackRfInterface() {
	delete transmittingData;
}

void HackRfInterface::setTXBuffer(CircleBufferNew<float>* txBuffer) {
	if (transmittingData != nullptr) transmittingData->setTXBuffer(txBuffer);
}

CircleBufferNew<float>* HackRfInterface::getTXBuffer() {
	return transmittingData->getTXBuffer();
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

void HackRfInterface::enableRxAmp(uint8_t amp) {
	if (savedRxAmp != amp) {
		savedRxAmp = amp;
		needToSetRxAmp = true;
	}
}

void HackRfInterface::enableTxAmp(uint8_t amp) {
	if (savedTxAmp != amp) {
		savedTxAmp = amp;
		needToSetTxAmp = true;
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
	HackRFDevice* hackRFDevice = (HackRFDevice*)device;

	hackRFDevice->config->transmit.txFreq = freq;

	bool result = ((HackRFDevice*)device)->startTX();

	if (result) {
		hackRFDevice->enableAmp(savedTxAmp);
	}

	return result;
}

bool HackRfInterface::stopTX() {

	HackRFDevice* hackRFDevice = (HackRFDevice*)device;

	bool result = ((HackRFDevice*)device)->stopTX();

	if (result) {
		hackRFDevice->enableAmp(savedRxAmp);
	}

	return result;
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

	if (needToSetRxAmp) {
		if (!isDeviceTransmitting()) {
			hackRFDevice->enableAmp(savedRxAmp);
			needToSetRxAmp = false;
		}
	}

	if (needToSetTxAmp) {
		if (isDeviceTransmitting()) {
			hackRFDevice->enableAmp(savedTxAmp);
			needToSetTxAmp = false;
		}
	}
}