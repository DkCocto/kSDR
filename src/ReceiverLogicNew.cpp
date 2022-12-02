#include "ReceiverLogicNew.h"

ReceiverLogicNew::ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre) {
	this->config = config;
	this->viewModel = viewModel;
	this->flowingFFTSpectre = flowingFFTSpectre;
}

//Устанавливает конкретную позицию линии приема получая в качестве параметра пиксель из ширины спектра
void ReceiverLogicNew::setPosition(float position, bool withoutDelta) {
	//this->position = position;
	//printf("%i\r\n", position);

	if (!withoutDelta) {
		this->position = position + delta;
	} else {
		this->position = position;
	}
	if (this->position <= 0) {
		this->position = 0;
		selectedBin = flowingFFTSpectre->getA();
	} else if (this->position >= spectreWidth) {
		this->position = spectreWidth;
		selectedBin = flowingFFTSpectre->getB();
	} else {
		selectedBin = (float)flowingFFTSpectre->getA() + (float)(this->position) / ((float)spectreWidth / (float)flowingFFTSpectre->getLen());
	}
	//Utils::printFloat(selectedBin);
}

void ReceiverLogicNew::updateSpectreWidth(float oldSpectreWidth, float newSpectreWidth) {
	this->position = this->position * newSpectreWidth / oldSpectreWidth;
	this->spectreWidth = newSpectreWidth;
}

void ReceiverLogicNew::syncFreq() {
	//Utils::printFloat(getSelectedFreq());
	setFreq(viewModel->centerFrequency + getSelectedFreq());
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

	int totalBin = flowingFFTSpectre->getLen();

	switch (receiverMode) {
	case USB:
		A = selectedBin - flowingFFTSpectre->getA();
		B = (position + filterWidthPX) / (spectreWidth / totalBin);
		if (B > totalBin) B = totalBin;
		break;
	case LSB:
		A = (position - filterWidthPX) / (spectreWidth / totalBin);
		B = selectedBin - flowingFFTSpectre->getA();
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

void ReceiverLogicNew::setFreq(float freq) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqsRangeAbsolute();
	FlowingFFTSpectre::FREQ_RANGE freqSamplerateRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();

	if (freq >= freqRange.first && freq <= freqRange.second) {
		float visibleSamplerateWidth = (freqSamplerateRange.second - freqSamplerateRange.first);

		float deltaFreq = freq - ((freqRange.first + visibleSamplerateWidth / 2.0) - visibleSamplerateWidth / 2.0);
		setPosition((deltaFreq * spectreWidth) / visibleSamplerateWidth, true);
	}
}

float ReceiverLogicNew::getFilterWidthAbs(int filterWidth) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();
	return ((float)filterWidth * spectreWidth) / (freqRange.second - freqRange.first);
}

float ReceiverLogicNew::getSelectedFreq() {
	float totalBin = flowingFFTSpectre->getAbsoluteSpectreLen();
	if (this != NULL) {
		if (selectedBin >= totalBin / 2.0) {
			return ((float)selectedBin * (float)config->inputSamplerate / (float)totalBin) - (float)config->inputSamplerate / 2.0;
		}
		else {
			return -1.0 * ((float)config->inputSamplerate / 2.0 - ((float)selectedBin * (float)config->inputSamplerate / (float)totalBin));
		}
	}
	else return 0.0;
}