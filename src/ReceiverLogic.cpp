#include "ReceiverLogic.h"

ReceiverLogic::ReceiverLogic(Config* config, int windowWidth) {
	this->config = config;
	totalBin = config->fftLen / 2.0;
	selectedBin = totalBin;
	init(windowWidth);
}

void ReceiverLogic::init(int windowWidth) {
	stepX = 2.0 / windowWidth;
	receiverPos = (windowWidth / 2.0);
	receiverPosByCoords = receiverPos * stepX - 1.0;
	selectedBin = (receiverPos) / (windowWidth / totalBin);
	this->windowWidth = windowWidth;
	initialized = true;
}

void ReceiverLogic::initAfterResize(int windowWidth) {
	stepX = 2.0 / windowWidth;
	receiverPos = getReseiverPos(windowWidth, selectedBin);
	receiverPosByCoords = receiverPos * stepX - 1.0;
	this->windowWidth = windowWidth;
}

float ReceiverLogic::getReseiverPos(int windowWidth, int selectedBin) {
	return selectedBin * (windowWidth / totalBin);
}

void ReceiverLogic::setAbsoluteXpos(int windowWidth, float pos) {
	this->absoluteXpos = pos;
	this->windowWidth = windowWidth;

	receiverPos = this->absoluteXpos + deltaXPos;

	if (receiverPos <= 0.0) {
		receiverPos = 0.0;
		receiverPosByCoords = -1.0;
		selectedBin = 0.0;
	} else if (receiverPos >= windowWidth) {
		receiverPos = windowWidth;
		receiverPosByCoords = 1.0;
		selectedBin = totalBin;
	} else {
		receiverPosByCoords = receiverPos * stepX - 1;
		selectedBin = (receiverPos) / (this->windowWidth / totalBin);
	}
}

void ReceiverLogic::storeDeltaXPos(float mouseX) {
	deltaXPos = receiverPos - mouseX;
	//printf("%f\r\n", deltaXPos);
}

int ReceiverLogic::getSelectedFreq() {
	if (this != NULL) {
		if (selectedBin >= totalBin / 2) {
			return (selectedBin * config->inputSamplerate / totalBin) - config->inputSamplerate / 2;
		} else {
			return -1 * (config->inputSamplerate / 2 - (selectedBin * config->inputSamplerate / totalBin));
		}
	}
	else return 0;
}

float ReceiverLogic::getFilterWidthAbs(int filterWidth) {
	//Абсолютная ширина спектра 
	float width_ = 2.0;

	return ((float)filterWidth * width_) / config->inputSamplerate;
}

int ReceiverLogic::getFilterWidthPX(int filterWidth) {
	//Абсолютная ширина спектра 
	float width_ = windowWidth;

	return (int)(((float)filterWidth * windowWidth) / config->inputSamplerate);
}

ReceiverLogic::ReceiveBinArea ReceiverLogic::getReceiveBinsArea(int filterWidth, int receiverMode) {
	int A = 0, B = 0;
	
	int filterWidthPX = getFilterWidthPX(filterWidth);

	switch (receiverMode) {
		case USB:
			A = selectedBin;
			B = (receiverPos + filterWidthPX) / (this->windowWidth / totalBin);
			if (B > totalBin) B = totalBin;
			break;
		case LSB:
			A = (receiverPos - filterWidthPX) / (this->windowWidth / totalBin);
			B = selectedBin;
			if (A < 0) A = 0;
			break;
		case AM:
			A = (receiverPos - filterWidthPX) / (this->windowWidth / totalBin);
			B = (receiverPos + filterWidthPX) / (this->windowWidth / totalBin);
			if (A < 0) A = 0;
			if (B > totalBin) B = totalBin;
			break;
	}

	ReceiveBinArea r{ A , B };
	return r;
}