#include "ViewModel.h"

void ViewModel::storeToConfig() {
	config->waterfallMin = waterfallMin;
	config->waterfallMax = waterfallMax;
	config->spectreRatio = ratio;
	config->spectreMin = minDb;
	config->startFrequency = centerFrequency;
	config->volume = volume;
	config->filterWidth = filterWidth;
	config->receiver.modulation = receiverMode;
	config->rsp.gain = rspModel.gain;
	config->rsp.lna = rspModel.lna;

	config->removeDCBias = removeDCBias;

	config->hackrf.lnaGain = hackRFModel.lnaGain;
	config->hackrf.vgaGain = hackRFModel.vgaGain;
	config->hackrf.txVgaGain = hackRFModel.txVgaGain;
	config->hackrf.rxAmp = hackRFModel.enableAmp;
}

void ViewModel::loadFromConfig() {
	centerFrequency = config->startFrequency;
	filterWidth = config->defaultFilterWidth;
	volume = config->volume;
	goToFreq = centerFrequency;


	//from xml
	waterfallMin = config->waterfallMin;
	waterfallMax = config->waterfallMax;
	ratio = config->spectreRatio;
	minDb = config->spectreMin;
	hackRFModel.lnaGain = config->hackrf.lnaGain;
	hackRFModel.vgaGain = config->hackrf.vgaGain;
	hackRFModel.txVgaGain = config->hackrf.txVgaGain;
	hackRFModel.enableAmp = config->hackrf.rxAmp;
	filterWidth = config->filterWidth;
	receiverMode = config->receiver.modulation;
	rspModel.gain = config->rsp.gain;
	rspModel.lna = config->rsp.lna;

	removeDCBias = config->removeDCBias;
}

ViewModel::ViewModel(Config* config) {
	this->config = config;
	loadFromConfig();
}

ViewModel::~ViewModel() {
	storeToConfig();
}

void ViewModel::setBufferAvailable(int readAvailableBufferCount) {
	bufferAvailable = (float)readAvailableBufferCount / (config->currentWorkingInputSamplerate / config->inputSamplerateDivider);
}