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
//¬озвращает сдвиг частоты на который надо сдвинуть центрульную частоту приема 
float FlowingFFTSpectre::move(int delta) {
	if (delta > 0) {
		if (B + delta > fftSH->getSpectreSize() - 1) {

			int bAfterDelta = B + delta;

			A += (fftSH->getSpectreSize() - 1) - B;
			B = fftSH->getSpectreSize() - 1;

			//Ѕудем сдвигать центральную частоту вперед
			//узнаЄм на сколько по частоте мы выходим за границы спектра
			float binsDelta = bAfterDelta - (fftSH->getSpectreSize() - 1);
			return getFreqOfOneSpectreBin() * binsDelta;
		} else {
			A += delta;
			B += delta;
		}
	}
	if (delta < 0) {
		delta *= -1;
		if (A - delta < 0) {

			int aAfterDelta = A - delta;

			B -= A;
			A = 0;

			//Ѕудем сдвигать центральную частоту назад
			//узнаЄм на сколько по частоте мы выходим за границы спектра
			float binsDelta = aAfterDelta;
			return getFreqOfOneSpectreBin() * binsDelta;
		} else {
			A -= delta;
			B -= delta;
		}
	}
	if (DEBUG) printCurrentPos();
	return 0.0;
}

float FlowingFFTSpectre::move(SPECTRE_POSITION fromSpectrePosition, int delta) {
	A = fromSpectrePosition.A;
	B = fromSpectrePosition.B;
	return move(delta);
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

float FlowingFFTSpectre::getAbsoluteFreqBySpectrePos(int pos) {
	return (float)viewModel->centerFrequency - ((float)config->inputSamplerate / 2.0) + getFreqByPosFromSamplerate(pos);
}

float FlowingFFTSpectre::getFreqByPosFromSamplerate(int pos) {
	return (float)pos * ((float)config->inputSamplerate / (float)fftSH->getSpectreSize());
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

void FlowingFFTSpectre::prepareForMovingSpectreByMouse(float mouseSpectrePos) {
	this->savedMouseSpectrePos = mouseSpectrePos;
	this->savedPosition.A = getA();
	this->savedPosition.B = getB();
	this->savedCenterFreq = viewModel->centerFrequency;
}

//¬озвращает новую центральную частоту приЄма
float FlowingFFTSpectre::moveSpectreByMouse(float spectreWidthInPx, float mouseSpectrePos) {
	float delta = savedMouseSpectrePos - mouseSpectrePos;
	FlowingFFTSpectre::FREQ_RANGE freqRange = getVisibleFreqRangeFromSamplerate();
	float spectreFreqWidth = freqRange.second - freqRange.first;
	
	float freqWidthByOnePx = spectreFreqWidth / spectreWidthInPx;
	
	float deltaFreqWidth = delta * freqWidthByOnePx;
	float countSpectreBinsInDelta = deltaFreqWidth / getFreqOfOneSpectreBin();

	return savedCenterFreq + move(savedPosition, countSpectreBinsInDelta);
}

float FlowingFFTSpectre::getFreqOfOneSpectreBin() {
	FlowingFFTSpectre::FREQ_RANGE freqRange = getVisibleFreqRangeFromSamplerate();
	float spectreFreqWidth = freqRange.second - freqRange.first;
	return spectreFreqWidth / getLen();
}