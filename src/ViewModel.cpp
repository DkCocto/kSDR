#include "ViewModel.h"

ViewModel::ViewModel(Config* config) {
	this->config = config;
	centerFrequency = config->startFrequency;
	filterWidth = config->defaultFilterWidth;
	volume = config->srartVolume;
}