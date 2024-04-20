#include "device/HackRFInterface.h"

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

	if (needToSetBaseband) {
		hackRFDevice->setBaseband(savedBaseband);
		needToSetBaseband = false;
	}

	if (needToSetAmp) {
		hackRFDevice->enableAmp(savedAmp);
		needToSetAmp = false;
	}
}