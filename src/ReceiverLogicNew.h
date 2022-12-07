#pragma once

#include "Env.h"
#include "math.h"
#include "Config.h"
#include "ViewModel.h"
#include "FlowingFFTSpectre.h"

class ReceiverLogicNew {
private:
	Config* config;
	ViewModel* viewModel;
	FlowingFFTSpectre* flowingFFTSpectre;

	float spectreWidthPx = 0; //������ ������� � ��������

	float centerFrequency = 0; //receiver frequency 
	float frequencyDelta = 0; //from  -(samplerate / 2) to samplerate/2
	float receiverPosOnPx = 0; //������� �� ������� ��������� ������ ����� �� ������� � �� ���� ������� ������� ������������� �������
	float receiverPosOnBin = 0; //������� �� ������� ��������� ������ ����� � ���� ������ spectre data bin 
	int savedPositionDelta = 0;

	//float spectreWidth = 0; //������ ������� � ��������
	//������� ����� �� ������� �������
	//float position = 0;

	//int selectedBin = 0;

	//int delta = 0;
	



public:

	ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre);

	void setFreq(float freq);

	void setCenterFrequency(float frequency);
	float getCenterFrequency();

	void setFrequencyDelta(float frequencyDelta);
	void setFrequencyDeltaBySpectrePosPx(float positionInSpectrePx);
	void setFrequencyDeltaFromSavedPosition(float positionInSpectrePx);
	float getFrequencyDelta();

	void saveSpectrePositionDelta(float position);

	float getPositionPX();

	float getSelectedFreqNew();

	float getPositionOnBin();

	//void setPosition(float position, bool withoutDelta);

	void updateSpectreWidth(float oldSpectreWidth, float newSpectreWidth);

	float getFilterWidthAbs(int filterWidth);

	//float getSelectedFreq();

	//int getPosition();
	//void saveDelta(int x);

	struct ReceiveBinArea {
		int A;
		int B;
	};

	ReceiveBinArea getReceiveBinsArea(int filterWidth, int receiverMode);

	//void setFreq(float freq);

};