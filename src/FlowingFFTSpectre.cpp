#include "FlowingFFTSpectre.h"

FlowingFFTSpectre::FlowingFFTSpectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH) {
	this->config = config;
	this->fftSH = fftSH;
	this->viewModel = viewModel;
	A = config->startBin;
	B = config->stopBin;
	if (DEBUG) printCurrentPos();
}

FlowingFFTSpectre::~FlowingFFTSpectre() {
	config->startBin = A;
	config->stopBin = B;
}

FFTData::OUTPUT* FlowingFFTSpectre::getDataCopy(FFTData::OUTPUT* spectreData) {
	return fftSH->getFFTData()->getDataCopy(spectreData, A, getLen(), false);
}

/*FFTData::OUTPUT* FlowingFFTSpectre::getWaterfallDataCopy(FFTData::OUTPUT* waterfallData) {
	return fftSH->getFFTData()->getDataCopy(waterfallData, A, getLen(), true);
}*/

void FlowingFFTSpectre::setPos(int A, int B) {
	if (A < 0) A = 0;
	if (B > config->fftLen / 2 - 1) B = config->fftLen / 2 - 1;
	this->A = A;
	this->B = B;
}

int FlowingFFTSpectre::getLen() {
	return (B - A) + 1;
}

int FlowingFFTSpectre::getAbsoluteSpectreLen() {
	return config->fftLen / 2;
}

//—двинуть спектр на количество единиц вперед или назад. «ависит от знака параметра delta
//¬озвращает сдвиг частоты на который надо сдвинуть центрульную частоту приема 
float FlowingFFTSpectre::move(int delta) {
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
	if (DEBUG) printCurrentPos();
}

void FlowingFFTSpectre::zoomOut(int step) {
	if (A - step < 0) A = 0;
	else A -= step;
	if (B + step > config->fftLen / 2 - 1) B = config->fftLen / 2 - 1;
	else B += step;

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
	printf("A=%i B=%i Len=%i, Total spectre[0; %i] Len: %i\r\n", A, B, getLen(), config->fftLen / 2 - 1, config->fftLen / 2);
}

float FlowingFFTSpectre::getAbsoluteFreqBySpectrePos(int pos) {
	return (float)viewModel->centerFrequency - ((float)config->inputSamplerate / 2.0) + getFreqByPosFromSamplerate(pos);
}

float FlowingFFTSpectre::getFreqByPosFromSamplerate(int pos) {
	return (float)pos * ((float)config->inputSamplerate / (float)(config->fftLen / 2));
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

/// <summary>
/// Returns the frequency range that the receiver is currently digitizing.
/// </summary>
/// <returns></returns>
FlowingFFTSpectre::FREQ_RANGE FlowingFFTSpectre::getTotalFreqsRange() {
	FREQ_RANGE freqRange {};
	freqRange.first = (float)(viewModel->centerFrequency - (config->inputSamplerate / 2));
	freqRange.second = (float)(viewModel->centerFrequency + (config->inputSamplerate / 2));
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
	return spectreFreqWidth / (float)getLen();
}

std::vector<float> FlowingFFTSpectre::getReducedData(FFTData::OUTPUT* fullSpectreData, int desiredBins) {
	
	int flowingSpectreLen = getLen();

	if (flowingSpectreLen <= desiredBins) {
		FFTData::OUTPUT* reducedSpectreData = getDataCopy(fullSpectreData);
		//int spectreLen = flowingSpectreLen;
		std::vector<float> v;
		v.assign(reducedSpectreData->data, reducedSpectreData->data + reducedSpectreData->len);
		fftSH->getFFTData()->destroyData(reducedSpectreData);
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
