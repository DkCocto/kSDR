#include "ViewModel.h"

ViewModel::ViewModel(Config* config) {
	this->config = config;
	centerFrequency = config->startFrequency;
	filterWidth = config->defaultFilterWidth;
	volume = config->volume;


	//from xml
	waterfallMin = config->waterfallMin;
	waterfallMax = config->waterfallMax;
	ratio = config->spectreRatio;
	minDb = config->spectreMin;
	spectreSpeed = config->spectreSpeed;
}

ViewModel::~ViewModel() {
	config->waterfallMin = waterfallMin;
	config->waterfallMax = waterfallMax;
	config->spectreRatio = ratio;
	config->spectreMin = minDb;
	config->spectreSpeed = spectreSpeed;
	config->startFrequency = centerFrequency;
	config->volume = volume;
}

void ViewModel::setBufferAvailable(int readAvailableBufferCount) {
	bufferAvailable = (float)readAvailableBufferCount / (config->inputSamplerate / config->inputSamplerateDivider);
}