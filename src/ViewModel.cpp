#include "ViewModel.h"

ViewModel::ViewModel(Config* config) {
	this->config = config;
	frequency = config->startFrequency;
	filterWidth = config->defaultFilterWidth;
	volume = config->srartVolume;
}