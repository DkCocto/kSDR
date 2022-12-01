#include "FlowingFFTSpectre.h"

FlowingFFTSpectre::FlowingFFTSpectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH) {
	this->config = config;
	this->fftSH = fftSH;
	this->viewModel = viewModel;
	A = 0;
	B = fftSH->getSpectreSize() - 1;
	if (DEBUG) printCurrentPos();
}

float* FlowingFFTSpectre::getData() {
	return fftSH->getOutputCopy(A, getLen());
}

void FlowingFFTSpectre::setPos(int A, int B) {
	if (A < 0) A = 0;
	if (B > fftSH->getSpectreSize() - 1) B = fftSH->getSpectreSize() - 1;
	this->A = A;
	this->B = B;
}

int FlowingFFTSpectre::getLen() {
	return (B - A) + 1;
}

int FlowingFFTSpectre::getAbsoluteSpectreLen() {
	return fftSH->getSpectreSize();
}

//—двинуть спектр на количество единиц вперед или назад. «ависит от знака параметра delta
void FlowingFFTSpectre::move(int delta) {
	if (delta > 0) {
		if (B + delta > fftSH->getSpectreSize() - 1) {
			A += (fftSH->getSpectreSize() - 1) - B;
			B = fftSH->getSpectreSize() - 1;
		} else {
			A += delta;
			B += delta;
		}
	}
	if (delta < 0) {
		delta *= -1;
		if (A - delta < 0) {
			B -= A;
			A = 0;
		} else {
			A -= delta;
			B -= delta;
		}
	}
	if (DEBUG) printCurrentPos();
}

void FlowingFFTSpectre::zoomIn(int step) {
	if (A + step < B - step) {
		A += step;
		B -= step;
	}
	if (DEBUG) printCurrentPos();
}

void FlowingFFTSpectre::zoomOut(int step) {
	if (A - step < 0) A = 0;
	else A -= step;
	if (B + step > fftSH->getSpectreSize() - 1) B = fftSH->getSpectreSize() - 1;
	else B += step;
	if (DEBUG) printCurrentPos();
}

FFTSpectreHandler* FlowingFFTSpectre::getSpectreHandler() {
	return fftSH;
}

void FlowingFFTSpectre::printCurrentPos() {
	printf("A=%i B=%i Len=%i, Total spectre[0; %i] Len: %i\r\n", A, B, getLen(), fftSH->getSpectreSize() - 1, fftSH->getSpectreSize());
}

float FlowingFFTSpectre::getAbsoluteFreqBySpectrePos(float pos) {
	return (float)viewModel->centerFrequency - (((float)config->inputSamplerate) / 2.0) + getFreqByPosFromSamplerate(pos);
}

float FlowingFFTSpectre::getFreqByPosFromSamplerate(float pos) {
	return pos * ((float)config->inputSamplerate / (float)fftSH->getSpectreSize());
}

FlowingFFTSpectre::FREQ_RANGE FlowingFFTSpectre::getVisibleFreqRangeFromSamplerate() {
	FREQ_RANGE freqRange{};
	freqRange.first = getFreqByPosFromSamplerate(A);
	freqRange.second = getFreqByPosFromSamplerate(B + 1);
	return freqRange;
}

FlowingFFTSpectre::FREQ_RANGE FlowingFFTSpectre::getVisibleFreqsRangeAbsolute() {
	FREQ_RANGE freqRange{};
	freqRange.first = getAbsoluteFreqBySpectrePos(A);
	freqRange.second = getAbsoluteFreqBySpectrePos(B + 1);
	return freqRange;
}

float FlowingFFTSpectre::getVisibleStartFrequency() {
	return getAbsoluteFreqBySpectrePos(A);
}

int FlowingFFTSpectre::getA() {
	return A;
}

int FlowingFFTSpectre::getB() {
	return B;
}
