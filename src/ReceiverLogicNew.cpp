#include "ReceiverLogicNew.h"

void ReceiverLogicNew::setCenterFrequency(float frequency) {
	this->centerFrequency = frequency;
}

float ReceiverLogicNew::getCenterFrequency() {
	return this->centerFrequency;
}

void ReceiverLogicNew::setFrequencyDelta(float frequencyDelta) {
	this->frequencyDelta = frequencyDelta;

	//float freqWidthOfOnePx = spectreWidthPx / config->inputSamplerate;

	float freqFromZero = config->inputSamplerate / 2 + frequencyDelta;

	FlowingFFTSpectre::FREQ_RANGE range = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();

	receiverPosOnBin = round(freqFromZero * (float)flowingFFTSpectre->getLen() / (range.second - range.first));

	receiverPosOnPx = round((receiverPosOnBin - flowingFFTSpectre->getA()) * spectreWidthPx / flowingFFTSpectre->getLen());

	//viewModel->serviceField1 = receiverPosOnBin;
}

void ReceiverLogicNew::setFrequencyDeltaBySpectrePosPx(float positionInSpectrePx) {
	receiverPosOnPx = positionInSpectrePx;
	receiverPosOnBin = round((float)flowingFFTSpectre->getA() + (float)receiverPosOnPx / ((float)spectreWidthPx / (float)flowingFFTSpectre->getLen()));
	

	float pos = receiverPosOnBin * flowingFFTSpectre->getFreqOfOneSpectreBin();
	if (pos > config->inputSamplerate / 2) frequencyDelta = pos - config->inputSamplerate / 2;
	else if (pos < config->inputSamplerate / 2) frequencyDelta = (-1) * (config->inputSamplerate / 2 - pos);
	else frequencyDelta = 0;
}

void ReceiverLogicNew::saveSpectrePositionDelta(float position) {
	this->savedPositionDelta = this->receiverPosOnPx - position;
}

float ReceiverLogicNew::getPositionPX() {
	return receiverPosOnPx;
}

float ReceiverLogicNew::getSelectedFreqNew() {
	return centerFrequency + frequencyDelta;
}

float ReceiverLogicNew::getPositionOnBin() {
	return receiverPosOnBin;
}

void ReceiverLogicNew::setFrequencyDeltaFromSavedPosition(float positionInSpectrePx) {
	int pos = positionInSpectrePx + savedPositionDelta;
	if (pos <= 0) {
		pos = 0;
	}
	else if (pos >= spectreWidthPx) {
		pos = spectreWidthPx;
	}
	setFrequencyDeltaBySpectrePosPx(pos);
}

float ReceiverLogicNew::getFrequencyDelta() {
	return frequencyDelta;
}

ReceiverLogicNew::ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre) {
	this->config = config;
	this->centerFrequency = config->startFrequency;
	this->viewModel = viewModel;
	this->flowingFFTSpectre = flowingFFTSpectre;
}

void ReceiverLogicNew::setFreq(float freq) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqsRangeAbsolute();
	if (freq >= freqRange.first && freq <= freqRange.second) {
		float delta = freq - centerFrequency;
		setFrequencyDelta(delta);
	}
}

//Устанавливает конкретную позицию линии приема получая в качестве параметра пиксель из ширины спектра
/*void ReceiverLogicNew::setPosition(float position, bool withoutDelta) {
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
		selectedBin = round((float)flowingFFTSpectre->getA() + (float)this->position / ((float)spectreWidth / (float)flowingFFTSpectre->getLen()));
	}
	
}*/

void ReceiverLogicNew::updateSpectreWidth(float oldSpectreWidth, float newSpectreWidth) {
	receiverPosOnPx = receiverPosOnPx * newSpectreWidth / oldSpectreWidth;
	this->spectreWidthPx = newSpectreWidth;
}

/*int ReceiverLogicNew::getPosition() {
	//printf("%i\r\n", spectreStartPosX);
	return position;
}*/

/*void ReceiverLogicNew::saveDelta(int x) {
	delta = position - x;
}*/

ReceiverLogicNew::ReceiveBinArea ReceiverLogicNew::getReceiveBinsArea(int filterWidth, int receiverMode) {
	int A = 0, B = 0;

	int filterWidthPX = getFilterWidthAbs(filterWidth);

	int totalBin = flowingFFTSpectre->getLen();

	switch (receiverMode) {
	case USB:
		A = receiverPosOnBin - flowingFFTSpectre->getA();
		B = (receiverPosOnPx + filterWidthPX) / (spectreWidthPx / totalBin);
		if (B > totalBin) B = totalBin;
		break;
	case LSB:
		A = (receiverPosOnPx - filterWidthPX) / (spectreWidthPx / totalBin);
		B = receiverPosOnBin - flowingFFTSpectre->getA();
		if (A < 0) A = 0;
		break;
	case AM:
		A = (receiverPosOnPx - filterWidthPX) / (spectreWidthPx / totalBin);
		B = (receiverPosOnPx + filterWidthPX) / (spectreWidthPx / totalBin);
		if (A < 0) A = 0;
		if (B > totalBin) B = totalBin;
		break;
	}

	ReceiveBinArea r{ A , B };
	return r;
}

/*void ReceiverLogicNew::setFreq(float freq) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqsRangeAbsolute();
	FlowingFFTSpectre::FREQ_RANGE freqSamplerateRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();

	if (freq >= freqRange.first && freq <= freqRange.second) {
		float visibleSamplerateWidth = (freqSamplerateRange.second - freqSamplerateRange.first);

		float deltaFreq = freq - ((freqRange.first + visibleSamplerateWidth / 2.0) - visibleSamplerateWidth / 2.0);

		setPosition((deltaFreq * spectreWidth) / visibleSamplerateWidth, true);
	}
}*/

float ReceiverLogicNew::getFilterWidthAbs(int filterWidth) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();
	return ((float)filterWidth * spectreWidthPx) / (freqRange.second - freqRange.first);
}

/*float ReceiverLogicNew::getSelectedFreq() {
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
}*/