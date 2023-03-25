#include "FlowingFFTSpectre.h"

FlowingSpectre::FlowingSpectre(Config* config, ViewModel* viewModel) {
	this->config = config;
	this->viewModel = viewModel;
	A = config->startBin;
	B = config->stopBin;
	if (DEBUG) printCurrentPos();
}

FlowingSpectre::~FlowingSpectre() {
	config->startBin = A;
	config->stopBin = B;
}

void FlowingSpectre::setPos(int A, int B) {
	if (A < 0) A = 0;
	if (B > config->fftLen / 2 - 1) B = config->fftLen / 2 - 1;
	this->A = A;
	this->B = B;
}

int FlowingSpectre::getLen() {
	return (B - A) + 1;
}

int FlowingSpectre::getAbsoluteSpectreLen() {
	return config->fftLen / 2;
}

//—двинуть спектр на количество единиц вперед или назад. «ависит от знака параметра delta
//¬озвращает сдвиг частоты на который надо сдвинуть центрульную частоту приема 
float FlowingSpectre::move(int delta) {
	if (delta > 0) {
		if (B + delta > config->fftLen / 2 - 1) {

			int bAfterDelta = B + delta;

			A += (config->fftLen / 2 - 1) - B;
			B = config->fftLen / 2 - 1;

			//Ѕудем сдвигать центральную частоту вперед
			//узнаЄм на сколько по частоте мы выходим за границы спектра
			float binsDelta = bAfterDelta - (config->fftLen / 2 - 1);
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

float FlowingSpectre::move(SPECTRE_POSITION fromSpectrePosition, int delta) {
	A = fromSpectrePosition.A;
	B = fromSpectrePosition.B;
	return move(delta);
}

void FlowingSpectre::zoomIn(int step) {
	if (getLen() <= 128) return;
	if (A + step < B - step) {
		A += step;
		B -= step;
	}
	if (DEBUG) printCurrentPos();
}

void FlowingSpectre::zoomOut(int step) {
	if (A - step < 0) A = 0;
	else A -= step;
	if (B + step > config->fftLen / 2 - 1) B = config->fftLen / 2 - 1;
	else B += step;

	if (DEBUG) printCurrentPos();
}

void FlowingSpectre::zoomIn() {
	zoomIn(getLen() / 8);
}

void FlowingSpectre::zoomOut() {
	zoomOut(getLen() / 8);
}

void FlowingSpectre::printCurrentPos() {
	printf("A=%i B=%i Len=%i, Total spectre[0; %i] Len: %i\r\n", A, B, getLen(), config->fftLen / 2 - 1, config->fftLen / 2);
}

float FlowingSpectre::getAbsoluteFreqBySpectrePos(int pos) {
	return (float)viewModel->centerFrequency - ((float)config->inputSamplerate / 2.0) + getFreqByPosFromSamplerate(pos);
}

float FlowingSpectre::getFreqByPosFromSamplerate(int pos) {
	return (float)pos * ((float)config->inputSamplerate / (float)(config->fftLen / 2));
}

FlowingSpectre::FREQ_RANGE FlowingSpectre::getVisibleFreqRangeFromSamplerate() {
	FREQ_RANGE freqRange{};
	freqRange.first = getFreqByPosFromSamplerate(A);
	freqRange.second = getFreqByPosFromSamplerate(B + 1);
	return freqRange;
}

FlowingSpectre::FREQ_RANGE FlowingSpectre::getVisibleFreqsRangeAbsolute() {
	FREQ_RANGE freqRange{};
	freqRange.first = getAbsoluteFreqBySpectrePos(A);
	freqRange.second = getAbsoluteFreqBySpectrePos(B + 1);
	return freqRange;
}

/// <summary>
/// Returns the frequency range that the receiver is currently digitizing.
/// </summary>
/// <returns></returns>
FlowingSpectre::FREQ_RANGE FlowingSpectre::getTotalFreqsRange() {
	FREQ_RANGE freqRange {};
	freqRange.first = (float)(viewModel->centerFrequency - (config->inputSamplerate / 2));
	freqRange.second = (float)(viewModel->centerFrequency + (config->inputSamplerate / 2));
	return freqRange;
}

float FlowingSpectre::getVisibleStartFrequency() {
	return getAbsoluteFreqBySpectrePos(A);
}

int FlowingSpectre::getA() {
	return A;
}

int FlowingSpectre::getB() {
	return B;
}

void FlowingSpectre::prepareForMovingSpectreByMouse(float mouseSpectrePos) {
	this->savedMouseSpectrePos = mouseSpectrePos;
	this->savedPosition.A = getA();
	this->savedPosition.B = getB();
	this->savedCenterFreq = viewModel->centerFrequency;
}

//¬озвращает новую центральную частоту приЄма
float FlowingSpectre::moveSpectreByMouse(float spectreWidthInPx, float mouseSpectrePos) {
	float delta = savedMouseSpectrePos - mouseSpectrePos;
	FlowingSpectre::FREQ_RANGE freqRange = getVisibleFreqRangeFromSamplerate();
	float spectreFreqWidth = freqRange.second - freqRange.first;
	
	float freqWidthByOnePx = spectreFreqWidth / spectreWidthInPx;
	
	float deltaFreqWidth = delta * freqWidthByOnePx;
	float countSpectreBinsInDelta = deltaFreqWidth / getFreqOfOneSpectreBin();

	return savedCenterFreq + move(savedPosition, countSpectreBinsInDelta);
}

float FlowingSpectre::getFreqOfOneSpectreBin() {
	FlowingSpectre::FREQ_RANGE freqRange = getVisibleFreqRangeFromSamplerate();
	float spectreFreqWidth = freqRange.second - freqRange.first;
	return spectreFreqWidth / (float)getLen();
}

std::vector<float> FlowingSpectre::getReducedData(FFTData::OUTPUT* fullSpectreData, int desiredBins, SpectreHandler* specHandler) {
	
	int flowingSpectreLen = getLen();

	if (flowingSpectreLen <= desiredBins) {
		FFTData::OUTPUT* reducedSpectreData = specHandler->getFFTData()->getDataCopy(fullSpectreData, A, getLen());
		//int spectreLen = flowingSpectreLen;
		std::vector<float> v;
		v.assign(reducedSpectreData->data, reducedSpectreData->data + reducedSpectreData->len);
		specHandler->getFFTData()->destroyData(reducedSpectreData);
		//delete[] spectreData;
		
		return v;
	}

	//return (value - From1) / (From2 - From1) * (To2 - To1) + To1;

	float koeff = (float)desiredBins / (float)flowingSpectreLen;

	int fullSpectreDataLen = fullSpectreData->len;

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
				reducedSpectreData.push_back(fullSpectreData->data[i]);
			}
			savedNewIndex = newIndex;
		}
	}

	return reducedSpectreData;
}
