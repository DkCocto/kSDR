#include "ReceiverLogicNew.h"

ReceiverLogicNew::ReceiverLogicNew(Config* config) {
	this->config = config;
	totalBin = config->fftLen / 2.0;
}

void ReceiverLogicNew::setPosition(float position, float spectreWidth) {
	//this->position = position;
	//printf("%i\r\n", position);

	this->position = position + delta;
	if (this->position <= 0) {
		this->position = 0;
		selectedBin = 0.0;
	} else if (this->position >= spectreWidth) {
		this->position = spectreWidth;
		selectedBin = totalBin;
	} else {
		selectedBin = (this->position) / (spectreWidth / totalBin);
	}
}

void ReceiverLogicNew::update(float oldSpectreWidth, float newSpectreWidth) {
	this->position = this->position * newSpectreWidth / oldSpectreWidth;
	this->spectreWidth = newSpectreWidth;
	//printf("%i %i %i\r\n", oldSpectreWidth, newSpectreWidth, this->position);
}

int ReceiverLogicNew::getPosition() {
	//printf("%i\r\n", spectreStartPosX);
	return position;
}

void ReceiverLogicNew::saveDelta(int x) {
	delta = position - x;
}

ReceiverLogicNew::ReceiveBinArea ReceiverLogicNew::getReceiveBinsArea(int filterWidth, int receiverMode) {
	int A = 0, B = 0;

	int filterWidthPX = getFilterWidthAbs(filterWidth);

	switch (receiverMode) {
	case USB:
		A = selectedBin;
		B = (position + filterWidthPX) / (spectreWidth / totalBin);
		if (B > totalBin) B = totalBin;
		break;
	case LSB:
		A = (position - filterWidthPX) / (spectreWidth / totalBin);
		B = selectedBin;
		if (A < 0) A = 0;
		break;
	case AM:
		A = (position - filterWidthPX) / (spectreWidth / totalBin);
		B = (position + filterWidthPX) / (spectreWidth / totalBin);
		if (A < 0) A = 0;
		if (B > totalBin) B = totalBin;
		break;
	}

	ReceiveBinArea r{ A , B };
	return r;
}

float ReceiverLogicNew::getFilterWidthAbs(int filterWidth) {
	return ((float)filterWidth * spectreWidth) / config->inputSamplerate;
}

int ReceiverLogicNew::getSelectedFreq() {
	if (this != NULL) {
		if (selectedBin >= totalBin / 2) {
			return (selectedBin * config->inputSamplerate / totalBin) - config->inputSamplerate / 2;
		}
		else {
			return -1 * (config->inputSamplerate / 2 - (selectedBin * config->inputSamplerate / totalBin));
		}
	}
	else return 0;
}

float ReceiverLogicNew::getSelectedBin() {
	return selectedBin;
}
