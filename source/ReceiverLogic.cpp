#include "ReceiverLogic.h"

void ReceiverLogic::setCenterFrequency(double frequency) {
	this->centerFrequency = frequency;
}

double ReceiverLogic::getCenterFrequency() {
	return this->centerFrequency;
}

/// <summary>
/// Set frequency delta. This is the delta taken from the center of the sample rate. It ranges from -samplerate/2 to +samplerate/2.
/// </summary>
/// <param name="frequencyDelta"></param>
void ReceiverLogic::setFrequencyDelta(double frequencyDelta) {
	this->frequencyDelta = frequencyDelta;

	double freqFromZero = config->inputSamplerate / 2.0 + frequencyDelta;

	FlowingSpectre::FREQ_RANGE range = flowingSpec->getVisibleFreqRangeFromSamplerate();

	receiverPosOnBin = round(freqFromZero * (double)flowingSpec->getLen() / (range.second - range.first));

	receiverPosOnPx = round((receiverPosOnBin - (double)flowingSpec->getA()) * (double)spectreWidthPx / (double)flowingSpec->getLen());
}

void ReceiverLogic::setFrequencyDeltaBySpectrePosPx(double positionInSpectrePx) {
	receiverPosOnPx = positionInSpectrePx;
	receiverPosOnBin = round((double)flowingSpec->getA() + (double)receiverPosOnPx / ((double)spectreWidthPx / (double)flowingSpec->getLen()));
	
	double pos = receiverPosOnBin * flowingSpec->getFreqOfOneSpectreBin();
	if (pos > (double)config->inputSamplerate / 2.0) frequencyDelta = pos - (double)config->inputSamplerate / 2.0;
	else if (pos < (double)config->inputSamplerate / 2.0) frequencyDelta = (-1.0) * ((double)config->inputSamplerate / 2.0 - pos);
	else frequencyDelta = 0.0;
}

void ReceiverLogic::saveSpectrePositionDelta(double position) {
	this->savedPositionDelta = this->receiverPosOnPx - position;
}

double ReceiverLogic::getPositionPX() {
	return receiverPosOnPx;
}

double ReceiverLogic::getSelectedFreqNew() {
	return centerFrequency + frequencyDelta;
}

double ReceiverLogic::getPositionOnBin() {
	return receiverPosOnBin;
}

void ReceiverLogic::setFrequencyDeltaFromSavedPosition(double positionInSpectrePx) {
	int pos = positionInSpectrePx + savedPositionDelta;
	if (pos <= 0) {
		pos = 0;
	}
	else if (pos >= spectreWidthPx) {
		pos = spectreWidthPx;
	}
	setFrequencyDeltaBySpectrePosPx((double)pos);
}

double ReceiverLogic::getFrequencyDelta() {
	return frequencyDelta;
}

ReceiverLogic::ReceiverLogic(Config* config, ViewModel* viewModel, FlowingSpectre* flowingSpec) {
	this->flowingSpec = flowingSpec;
	this->config = config;
	this->centerFrequency = config->startFrequency;
	this->viewModel = viewModel;
}

constexpr auto SHIFT = 200000.0f;
/// <summary>
/// Go to current freq. The received freq will be displayed in the middle of the spectrum.
/// </summary>
/// <param name="freq">frequency</param>
void ReceiverLogic::setFreq(double freq) {

	if (freq < 0) return;

	FlowingSpectre::FREQ_RANGE visibleFreqsRange = flowingSpec->getVisibleFreqsRangeAbsolute();
	if (freq >= visibleFreqsRange.first && freq <= visibleFreqsRange.second) {
		double delta = freq - centerFrequency;
		setFrequencyDelta(delta);
	} else {
		FlowingSpectre::FREQ_RANGE totalFreqsRange = flowingSpec->getTotalFreqsRange();
		//if (freq >= totalFreqsRange.first && freq <= totalFreqsRange.second) {
			//empty
		//} else {
			if (SHIFT < config->inputSamplerate / 2.0f) {
				viewModel->centerFrequency = freq - SHIFT;
			} else viewModel->centerFrequency = freq - config->inputSamplerate / 4.0f;
		//}

		double delta = freq - viewModel->centerFrequency;
		setFrequencyDelta(delta);

		int spectreLenBin = flowingSpec->getLen();
		int positionOnBin = getPositionOnBin();

		int totalSpectreSize = config->fftLen / 2;

		int deltaPos = spectreLenBin / 2;

		int B = (positionOnBin + deltaPos < totalSpectreSize) ? positionOnBin + deltaPos : totalSpectreSize;
		int A = (positionOnBin - deltaPos >= 0) ? positionOnBin - deltaPos : 0;

		flowingSpec->setPos(A, B);

		setFrequencyDelta(delta);
	}
}

void ReceiverLogic::updateSpectreWidth(double oldSpectreWidth, double newSpectreWidth) {
	receiverPosOnPx = receiverPosOnPx * newSpectreWidth / oldSpectreWidth;
	this->spectreWidthPx = newSpectreWidth;
}

ReceiverLogic::ReceiveBinArea ReceiverLogic::getReceiveBinsArea(int filterWidth, int receiverMode) {
	int A = 0, B = 0;

	int filterWidthPX = getFilterWidthAbs(filterWidth);

	int totalBin = config->fftLen / 2;

	int deltaBin = (int)((double)filterWidth / flowingSpec->getFreqOfOneSpectreBin());

	switch (receiverMode) {
		case USB:
			A = receiverPosOnBin;
			B = A + deltaBin; // (receiverPosOnPx + filterWidthPX) / (spectreWidthPx / totalBin);
			if (B > totalBin) B = totalBin;
			break;
		case LSB:
			B = receiverPosOnBin;
			A = B - deltaBin; // (receiverPosOnPx - filterWidthPX) / (spectreWidthPx / totalBin);
			if (A < 0) A = 0;
			break;
		case AM:
			A = (receiverPosOnBin) - deltaBin; // (receiverPosOnPx - filterWidthPX) / (spectreWidthPx / totalBin);
			B = (receiverPosOnBin) + deltaBin; //(receiverPosOnPx + filterWidthPX) / (spectreWidthPx / totalBin);
			if (A < 0) A = 0;
			if (B > totalBin) B = totalBin;
			break;
	}

	ReceiveBinArea r{ A , B };
	return r;
}

double ReceiverLogic::getFilterWidthAbs(int filterWidth) {
	FlowingSpectre::FREQ_RANGE freqRange = flowingSpec->getVisibleFreqRangeFromSamplerate();
	return ((double)filterWidth * spectreWidthPx) / (freqRange.second - freqRange.first);
}

double ReceiverLogic::getFreqByPosOnSpectrePx(int px) {
	FlowingSpectre::FREQ_RANGE freqRange = flowingSpec->getVisibleFreqsRangeAbsolute();
	double freq = round((int)freqRange.first + (px * (freqRange.second - freqRange.first)) / spectreWidthPx);
	return freq;
}

/// <summary>
/// Возвращает позицию на спектре в пикселях относительно заданной частоты
/// </summary>
/// <param name="freq"></param>
/// <returns>Позицию на спектре или -1 если частота в данный момент находится вне границ видимого спектра</returns>
int ReceiverLogic::getPosOnSpectreByFreq(double freq) {
	FlowingSpectre::FREQ_RANGE freqRange = flowingSpec->getVisibleFreqsRangeAbsolute();
	if (freq >= freqRange.first && freq <= freqRange.second) {
		return round(((int)spectreWidthPx * (freq - freqRange.first)) / (freqRange.second - freqRange.first));
	} else {
		return -1;
	}
}

void ReceiverLogic::setReceivedFreqToSpectreCenter() {
	int spectreLenBin = flowingSpec->getLen();
	int positionOnBin = getPositionOnBin();

	int totalSpectreSize = config->fftLen / 2;

	int deltaPos = spectreLenBin / 2;

	int B = (positionOnBin + deltaPos < totalSpectreSize) ? positionOnBin + deltaPos : totalSpectreSize;
	int A = (positionOnBin - deltaPos >= 0) ? positionOnBin - deltaPos : 0;

	flowingSpec->setPos(A, B);

	double delta = getSelectedFreqNew() - viewModel->centerFrequency;

	setFrequencyDelta(delta);
}