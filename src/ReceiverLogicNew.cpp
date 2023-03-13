#include "ReceiverLogicNew.h"

void ReceiverLogicNew::setCenterFrequency(double frequency) {
	this->centerFrequency = frequency;
}

double ReceiverLogicNew::getCenterFrequency() {
	return this->centerFrequency;
}

/// <summary>
/// Set frequency delta. This is the delta taken from the center of the sample rate. It ranges from -samplerate/2 to +samplerate/2.
/// </summary>
/// <param name="frequencyDelta"></param>
void ReceiverLogicNew::setFrequencyDelta(double frequencyDelta) {
	this->frequencyDelta = frequencyDelta;

	double freqFromZero = config->inputSamplerate / 2.0 + frequencyDelta;

	FlowingFFTSpectre::FREQ_RANGE range = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();

	receiverPosOnBin = round(freqFromZero * (double)flowingFFTSpectre->getLen() / (range.second - range.first));

	receiverPosOnPx = round((receiverPosOnBin - (double)flowingFFTSpectre->getA()) * (double)spectreWidthPx / (double)flowingFFTSpectre->getLen());
}

void ReceiverLogicNew::setFrequencyDeltaBySpectrePosPx(double positionInSpectrePx) {
	receiverPosOnPx = positionInSpectrePx;
	receiverPosOnBin = round((double)flowingFFTSpectre->getA() + (double)receiverPosOnPx / ((double)spectreWidthPx / (double)flowingFFTSpectre->getLen()));
	
	double pos = receiverPosOnBin * flowingFFTSpectre->getFreqOfOneSpectreBin();
	if (pos > (double)config->inputSamplerate / 2.0) frequencyDelta = pos - (double)config->inputSamplerate / 2.0;
	else if (pos < (double)config->inputSamplerate / 2.0) frequencyDelta = (-1.0) * ((double)config->inputSamplerate / 2.0 - pos);
	else frequencyDelta = 0.0;
}

void ReceiverLogicNew::saveSpectrePositionDelta(double position) {
	this->savedPositionDelta = this->receiverPosOnPx - position;
}

double ReceiverLogicNew::getPositionPX() {
	return receiverPosOnPx;
}

double ReceiverLogicNew::getSelectedFreqNew() {
	return centerFrequency + frequencyDelta;
}

double ReceiverLogicNew::getPositionOnBin() {
	return receiverPosOnBin;
}

void ReceiverLogicNew::setFrequencyDeltaFromSavedPosition(double positionInSpectrePx) {
	int pos = positionInSpectrePx + savedPositionDelta;
	if (pos <= 0) {
		pos = 0;
	}
	else if (pos >= spectreWidthPx) {
		pos = spectreWidthPx;
	}
	setFrequencyDeltaBySpectrePosPx((double)pos);
}

double ReceiverLogicNew::getFrequencyDelta() {
	return frequencyDelta;
}

ReceiverLogicNew::ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre) {
	this->config = config;
	this->centerFrequency = config->startFrequency;
	this->viewModel = viewModel;
	this->flowingFFTSpectre = flowingFFTSpectre;
}

constexpr auto SHIFT = 300000.0f;
/// <summary>
/// Go to current freq. The received freq will be displayed in the middle of the spectrum.
/// </summary>
/// <param name="freq">frequency</param>
void ReceiverLogicNew::setFreq(double freq) {

	if (freq < 0) return;

	FlowingFFTSpectre::FREQ_RANGE visibleFreqsRange = flowingFFTSpectre->getVisibleFreqsRangeAbsolute();
	if (freq >= visibleFreqsRange.first && freq <= visibleFreqsRange.second) {
		double delta = freq - centerFrequency;
		setFrequencyDelta(delta);
	} else {
		FlowingFFTSpectre::FREQ_RANGE totalFreqsRange = flowingFFTSpectre->getTotalFreqsRange();
		//if (freq >= totalFreqsRange.first && freq <= totalFreqsRange.second) {
			//empty
		//} else {
			if (SHIFT < config->inputSamplerate / 2.0f) {
				viewModel->centerFrequency = freq - SHIFT;
			} else viewModel->centerFrequency = freq - config->inputSamplerate / 4.0f;
		//}

		double delta = freq - viewModel->centerFrequency;
		setFrequencyDelta(delta);

		int spectreLenBin = flowingFFTSpectre->getLen();
		int positionOnBin = getPositionOnBin();

		int totalSpectreSize = flowingFFTSpectre->getSpectreHandler()->getSpectreSize();

		int deltaPos = spectreLenBin / 2;

		int B = (positionOnBin + deltaPos < totalSpectreSize) ? positionOnBin + deltaPos : totalSpectreSize;
		int A = (positionOnBin - deltaPos >= 0) ? positionOnBin - deltaPos : 0;

		flowingFFTSpectre->setPos(A, B);

		setFrequencyDelta(delta);
	}
}

void ReceiverLogicNew::updateSpectreWidth(double oldSpectreWidth, double newSpectreWidth) {
	receiverPosOnPx = receiverPosOnPx * newSpectreWidth / oldSpectreWidth;
	this->spectreWidthPx = newSpectreWidth;
}

ReceiverLogicNew::ReceiveBinArea ReceiverLogicNew::getReceiveBinsArea(int filterWidth, int receiverMode) {
	int A = 0, B = 0;

	int filterWidthPX = getFilterWidthAbs(filterWidth);

	int totalBin = flowingFFTSpectre->getSpectreHandler()->getSpectreSize();

	int deltaBin = (int)((double)filterWidth / flowingFFTSpectre->getFreqOfOneSpectreBin());

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

double ReceiverLogicNew::getFilterWidthAbs(int filterWidth) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();
	return ((double)filterWidth * spectreWidthPx) / (freqRange.second - freqRange.first);
}

double ReceiverLogicNew::getFreqByPosOnSpectrePx(int px) {
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqsRangeAbsolute();
	double freq = round((int)freqRange.first + (px * ((int)freqRange.second - (int)freqRange.first)) / (int)spectreWidthPx);
	return freq;
}

void ReceiverLogicNew::setReceivedFreqToSpectreCenter() {
	int spectreLenBin = flowingFFTSpectre->getLen();
	int positionOnBin = getPositionOnBin();

	int totalSpectreSize = flowingFFTSpectre->getSpectreHandler()->getSpectreSize();

	int deltaPos = spectreLenBin / 2;

	int B = (positionOnBin + deltaPos < totalSpectreSize) ? positionOnBin + deltaPos : totalSpectreSize;
	int A = (positionOnBin - deltaPos >= 0) ? positionOnBin - deltaPos : 0;

	flowingFFTSpectre->setPos(A, B);

	double delta = getSelectedFreqNew() - viewModel->centerFrequency;
	//printf("%f\n", delta);
	setFrequencyDelta(delta);
}