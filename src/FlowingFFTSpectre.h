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

	bool DEBUG = true;

public:

	FlowingFFTSpectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH);
	float* getData();
	void setPos(int A, int B);
	int getLen();
	int getAbsoluteSpectreLen();
	void move(int delta);
	void zoomIn(int step);
	void zoomOut(int step);
	FFTSpectreHandler* getSpectreHandler();
	void printCurrentPos();
	float getAbsoluteFreqBySpectrePos(float pos);
	float getFreqByPosFromSamplerate(float pos);
	struct FREQ_RANGE {
		float first;
		float second;
	};
	FREQ_RANGE getVisibleFreqRangeFromSamplerate();
	FREQ_RANGE getVisibleFreqsRangeAbsolute();
	float getVisibleStartFrequency();
	int getA();
	int getB();
};