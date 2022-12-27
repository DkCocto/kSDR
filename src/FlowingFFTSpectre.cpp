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
	if (getLen() <= 128) return;
	if (A + step < B - step) {
		A += step;
		B -= step;
	}

	/*int remainder = getLen() % config->visibleSpectreBinCount;
	if (remainder != 0) {
		if (B + remainder < fftSH->getSpectreSize() - 1) {
			B += remainder;
		}
	}*/

	if (DEBUG) printCurrentPos();
}

void FlowingFFTSpectre::zoomOut(int step) {
	if (A - step < 0) A = 0;
	else A -= step;
	if (B + step > fftSH->getSpectreSize() - 1) B = fftSH->getSpectreSize() - 1;
	else B += step;

	/*int remainder = getLen() % config->visibleSpectreBinCount;
	printf("%d\r\n", remainder);
	if (remainder != 0) {
		B -= remainder;
	}*/

	if (DEBUG) printCurrentPos();
}

void FlowingFFTSpectre::zoomIn() {
	zoomIn(getLen() / 8);
}

void FlowingFFTSpectre::zoomOut() {
	zoomOut(getLen() / 8);
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

std::vector<float> FlowingFFTSpectre::getReducedSpectre(float* fullSpectreData, int fullSpectreDataLen, int desiredBins) {
	
	if (getLen() <= desiredBins) {
		float* spectreData = getData();
		int spectreLen = getLen();
		std::vector<float> v;
		v.assign(spectreData, spectreData + spectreLen);
		return v;
	}

	//return (value - From1) / (From2 - From1) * (To2 - To1) + To1;

	float koeff = (float)desiredBins / (float)getLen();

	int newSpectreDataLen = (int)round(fullSpectreDataLen * koeff);

	std::vector<float> reducedSpectreData;

	int savedNewIndex = -1;
	//int indexCount = 0;

	int ANew = (int)round(A * koeff);
	int BNew = (int)round(B * koeff);

	for (int i = 0; i < fullSpectreDataLen; i++) {
		int newIndex = (int)round((float)i / fullSpectreDataLen * newSpectreDataLen);

		if (savedNewIndex != newIndex) {
			if (newIndex >= ANew && newIndex <= BNew) {
				reducedSpectreData.push_back(fullSpectreData[i]);
			}
			savedNewIndex = newIndex;
		}
	}

	/* int spectreLen = getLen();
	
	if (spectreLen % desiredBins != 0) {
		std::vector<float> v;
		v.assign(spectreData, spectreData + spectreLen);
		return v;
	}

	//float* reducedSpectreData = new float[desiredBins];
	//memset(reducedSpectreData, 0, sizeof(float) * desiredBins);
	std::vector<float> reducedSpectreData;

	int reducedDiv = spectreLen / desiredBins;

	//0  1  2  3  4  5  6  7   len = 8
	//0  1  2  3			   len = 4

	for (int i = 0; i < desiredBins; i++) {
		float sum = 0;
		for (int j = 0; j < reducedDiv; j++) {
			sum += spectreData[i * reducedDiv + j];
		}
		reducedSpectreData.push_back(sum / reducedDiv);
	}*/

	return reducedSpectreData;
}
