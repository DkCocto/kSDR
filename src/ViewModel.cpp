#include "ViewModel.h"

void ViewModel::storeToConfig() {
	config->waterfall.value1 = waterfall.value1;
	config->waterfall.value2 = waterfall.value2;
	config->waterfall.value3 = waterfall.value3;
	config->waterfall.value4 = waterfall.value4;
	config->waterfall.value5 = waterfall.value5;
	config->waterfall.value6 = waterfall.value6;
	config->waterfall.value7 = waterfall.value7;
	config->waterfall.value8 = waterfall.value8;
	config->waterfall.value9 = waterfall.value9;
	config->waterfall.value10 = waterfall.value10;



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

	config->hackrf.lnaGain = hackRF.lnaGain;
	config->hackrf.vgaGain = hackRF.vgaGain;
	config->hackrf.txVgaGain = hackRF.txVgaGain;
	config->hackrf.rxAmp = hackRF.rxAmp;
	config->hackrf.txAmp = hackRF.txAmp;

	config->transmit.txBySpaceBtn = transmit.txBySpaceBtn;
	config->transmit.amModulationDepth = transmit.amModulationDepth;
	config->transmit.inputLevel = transmit.inputLevel;
	config->transmit.inputLevel2 = transmit.inputLevel2;
	config->transmit.inputLevel3 = transmit.inputLevel3;
	config->transmit.sendToneSignal = transmit.sendToneSignal;
	config->transmit.tone1Freq = transmit.tone1Freq;
	config->transmit.tone2Freq = transmit.tone2Freq;
	config->transmit.outputPower = transmit.outputPower;
	config->transmit.phaserFilter = transmit.phaserFilter;


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
	hackRF.lnaGain = config->hackrf.lnaGain;
	hackRF.vgaGain = config->hackrf.vgaGain;
	hackRF.txVgaGain = config->hackrf.txVgaGain;
	hackRF.rxAmp = config->hackrf.rxAmp;
	hackRF.txAmp = config->hackrf.txAmp;

	transmit.txBySpaceBtn = config->transmit.txBySpaceBtn;
	transmit.amModulationDepth = config->transmit.amModulationDepth;
	transmit.inputLevel = config->transmit.inputLevel;
	transmit.inputLevel2 = config->transmit.inputLevel2;
	transmit.inputLevel3 = config->transmit.inputLevel3;
	transmit.sendToneSignal = config->transmit.sendToneSignal;
	transmit.outputPower = config->transmit.outputPower;
	transmit.phaserFilter = config->transmit.phaserFilter;
	
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

	waterfall.value1 = config->waterfall.value1;
	waterfall.value2 = config->waterfall.value2;
	waterfall.value3 = config->waterfall.value3;
	waterfall.value4 = config->waterfall.value4;
	waterfall.value5 = config->waterfall.value5;
	waterfall.value6 = config->waterfall.value6;
	waterfall.value7 = config->waterfall.value7;
	waterfall.value8 = config->waterfall.value8;
	waterfall.value9 = config->waterfall.value9;
	waterfall.value10 = config->waterfall.value10;

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