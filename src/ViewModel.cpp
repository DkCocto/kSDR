#include "ViewModel.h"

void ViewModel::storeToConfig() {
	config->waterfallMin = waterfallMin;
	config->waterfallMax = waterfallMax;
	config->spectreRatio = ratio;
	config->spectreMin = minDb;
	config->startFrequency = centerFrequency;
	config->volume = pow(2.718, volume) - 1;
	config->filterWidth = filterWidth;
	config->receiver.modulation = receiverMode;
	config->receiver.enableNotch = enableNotch;
	config->receiver.notchCenterFreq = notchCenterFreq;
	config->rsp.gain = rspModel.gain;
	config->rsp.lna = rspModel.lna;

	config->removeDCBias = removeDCBias;

	config->hackrf.lnaGain = hackRFModel.lnaGain;
	config->hackrf.vgaGain = hackRFModel.vgaGain;
	config->hackrf.txVgaGain = hackRFModel.txVgaGain;
	config->hackrf.rxAmp = hackRFModel.enableRxAmp;
	config->hackrf.txAmp = hackRFModel.enableTxAmp;

	config->transmit.txBySpaceBtn = transmit.txBySpaceBtn;
	config->transmit.amModulationDepth = transmit.amModulationDepth;
	config->transmit.inputLevel = transmit.inputLevel;
	config->transmit.inputLevel2 = transmit.inputLevel2;
	config->transmit.inputLevel3 = transmit.inputLevel3;
	config->transmit.sendToneSignal = transmit.sendToneSignal;
	config->transmit.tone1Freq = transmit.tone1Freq;
	config->transmit.tone2Freq = transmit.tone2Freq;

	config->myTranceiverDevice.att = myTranceiverDevice.att;
	config->myTranceiverDevice.pre = myTranceiverDevice.pre;
	config->myTranceiverDevice.bypass = myTranceiverDevice.bypass;

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
	hackRFModel.enableRxAmp = config->hackrf.rxAmp;
	hackRFModel.enableTxAmp = config->hackrf.txAmp;

	transmit.txBySpaceBtn = config->transmit.txBySpaceBtn;
	transmit.amModulationDepth = config->transmit.amModulationDepth;
	transmit.inputLevel = config->transmit.inputLevel;
	transmit.inputLevel2 = config->transmit.inputLevel2;
	transmit.inputLevel3 = config->transmit.inputLevel3;
	transmit.sendToneSignal = config->transmit.sendToneSignal;
	
	transmit.tone1Freq = config->transmit.tone1Freq;
	transmit.tone2Freq = config->transmit.tone2Freq;

	myTranceiverDevice.att = config->myTranceiverDevice.att;
	myTranceiverDevice.pre = config->myTranceiverDevice.pre;
	myTranceiverDevice.bypass = config->myTranceiverDevice.bypass;

	filterWidth = config->filterWidth;
	receiverMode = config->receiver.modulation;
	
	enableNotch = config->receiver.enableNotch;
	notchCenterFreq = config->receiver.notchCenterFreq;

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