#include "device/RSPInterface.h"

void RSPInterface::setFreq(double freq) {
	if (savedFreq != freq) {
		savedFreq = freq;
		needToSetFreq = true;
	}
}

void RSPInterface::setGain(int gain, unsigned char lnaState) {
	if (savedGain != gain || savedLnaState != lnaState) {
		savedGain = gain;
		savedLnaState = lnaState;
		needToSetGain = true;
	}
}

void RSPInterface::setBasebandFilter(int filterWidth) {
	if (savedBasebandFilter != filterWidth) {
		savedBasebandFilter = filterWidth;
		needToSetFilter = true;
	}
}

void RSPInterface::sendParamsToDevice() {
	RSPDevice* rspDevice = (RSPDevice*)device;
	if (needToSetFreq) {
		rspDevice->setFreq((savedFreq + ((rspDevice->config->receiver.enableFrequencyShift == true) ? (double)rspDevice->config->receiver.frequencyShift : 0.0)));
		needToSetFreq = false;
	}

	if (needToSetGain) {
		rspDevice->setGain(savedGain, savedLnaState);
		needToSetGain = false;
	}

	if (needToSetFilter) {
		rspDevice->setBasebandFilter(savedBasebandFilter);
		needToSetFilter = false;
	}
}
