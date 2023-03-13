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

	double spectreWidthPx = 0; //������ ������� � ��������

	double centerFrequency = 0; //receiver frequency 
	double frequencyDelta = 0; //from  -(samplerate / 2) to samplerate/2
	double receiverPosOnPx = 0; //������� �� ������� ��������� ������ ����� �� ������� � �� ���� ������� ������� ������������� �������
	double receiverPosOnBin = 0; //������� �� ������� ��������� ������ ����� � ���� ������ spectre data bin 
	int savedPositionDelta = 0;

public:

	ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre);

	void setFreq(double freq);

	void setCenterFrequency(double frequency);
	double getCenterFrequency();

	void setFrequencyDelta(double frequencyDelta);
	void setFrequencyDeltaBySpectrePosPx(double positionInSpectrePx);
	void setFrequencyDeltaFromSavedPosition(double positionInSpectrePx);
	double getFrequencyDelta();

	void saveSpectrePositionDelta(double position);

	double getPositionPX();

	double getSelectedFreqNew();

	double getPositionOnBin();

	void updateSpectreWidth(double oldSpectreWidth, double newSpectreWidth);

	double getFilterWidthAbs(int filterWidth);

	double getFreqByPosOnSpectrePx(int px);

	void setReceivedFreqToSpectreCenter();

	struct ReceiveBinArea {
		int A;
		int B;
	};

	ReceiveBinArea getReceiveBinsArea(int filterWidth, int receiverMode);
};