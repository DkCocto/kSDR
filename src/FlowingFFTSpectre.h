#pragma once

#include "FFTSpectreHandler.h"
#include "ViewModel.h"

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
	float* getData();
	void setPos(int A, int B);
	int getLen();
	int getAbsoluteSpectreLen();
	float move(int delta);
	float move(SPECTRE_POSITION fromSpectrePosition, int delta);
	void zoomIn(int step);
	void zoomOut(int step);
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
	float getVisibleStartFrequency();
	int getA();
	int getB();
	void prepareForMovingSpectreByMouse(float mouseSpectrePos);
	float moveSpectreByMouse(float spectreWidthInPx, float mouseSpectrePos);

	float getFreqOfOneSpectreBin();

};