#pragma once

#include "FFTSpectreHandler.h"
#include "ViewModel.h"
#include "vector"

constexpr auto WATERFALL = true;
constexpr auto SPECTRE = false;

//����� ���������� ��������� ������, ������� ����� �������� ������ ������� ������� FFT. � �������
//���� ����� ����� �������������� ������� ������� ������� ������ ������� �������
class FlowingSpectre {

	int A = 0;
	int B = 0;

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

	FlowingSpectre(Config* config, ViewModel* viewModel);
	~FlowingSpectre();
	//FFTData::OUTPUT* getDataCopy(FFTData::OUTPUT* spectreData);
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

	void printCurrentPos();
	int getSpectrePosByAbsoluteFreq(double freq);
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
	std::vector<float> getReducedData(FFTData::OUTPUT* fullSpectreData, int desiredBins, SpectreHandler* specHandler);

};