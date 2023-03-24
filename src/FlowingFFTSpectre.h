#pragma once

#include "FFTSpectreHandler.h"
#include "ViewModel.h"
#include "vector"

constexpr auto WATERFALL = true;
constexpr auto SPECTRE = false;

//Класс определяет плавающий спектр, который может меняться внутри полного спектра FFT. С помощью
//него можно будет контролировать масштаб кусочка спектра внутри полного спектра
class FlowingFFTSpectre {

	int A = 0;
	int B = 0;

	FFTSpectreHandler* fftSH;
	ViewModel* viewModel;
	Config* config;

	bool DEBUG = false;

	float savedMouseSpectrePos = 0;
	float savedCenterFreq = 0;

	struct SPECTRE_POSITION {
		int A;
		int B;
	};

	SPECTRE_POSITION savedPosition;

public:

	FlowingFFTSpectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH);
	~FlowingFFTSpectre();
	FFTData::OUTPUT* getDataCopy(FFTData::OUTPUT* spectreData);
	//FFTData::OUTPUT* getWaterfallDataCopy(FFTData::OUTPUT* waterfallData);
	void setPos(int A, int B);
	int getLen();
	int getAbsoluteSpectreLen();
	float move(int delta);
	float move(SPECTRE_POSITION fromSpectrePosition, int delta);
	void zoomIn(int step);
	void zoomOut(int step);

	void zoomIn();
	void zoomOut();

	FFTSpectreHandler* getSpectreHandler();
	void printCurrentPos();
	float getAbsoluteFreqBySpectrePos(int pos);
	float getFreqByPosFromSamplerate(int pos);
	struct FREQ_RANGE {
		float first;
		float second;
	};
	FREQ_RANGE getVisibleFreqRangeFromSamplerate();
	FREQ_RANGE getVisibleFreqsRangeAbsolute();
	FREQ_RANGE getTotalFreqsRange();
	float getVisibleStartFrequency();
	int getA();
	int getB();
	void prepareForMovingSpectreByMouse(float mouseSpectrePos);
	float moveSpectreByMouse(float spectreWidthInPx, float mouseSpectrePos);

	float getFreqOfOneSpectreBin();

	//void setReceivedFreqToSpectreCenter();

	//desiredBins should be div by 2
	std::vector<float> getReducedData(FFTData::OUTPUT* fullSpectreData, int desiredBins);

};