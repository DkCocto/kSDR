#include "device/RTLInterface.h"

void RTLInterface::setFreq(uint32_t freq) {
	if (savedFreq != freq) {
		savedFreq = freq;
		needToSetFreq = true;
	}
}

void RTLInterface::setGain(int gain) {
	if (savedGain != gain) {
		savedGain = gain;
		needToSetGain = gain;
	}
}

void RTLInterface::sendParamsToDevice() {
	RTLDevice* rtlDevice = (RTLDevice*)device;
	if (needToSetFreq) {
		rtlDevice->setFreq((savedFreq + ((rtlDevice->config->receiver.enableFrequencyShift == true) ? rtlDevice->config->receiver.frequencyShift : 0)));
		needToSetFreq = false;
	}

	if (needToSetGain) {
		rtlDevice->setGain(savedGain);
		needToSetGain = false;
	}
}