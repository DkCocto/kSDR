#pragma once

#include "Env.h"
#include "math.h"
#include "Config.h"
#include "ViewModel.h"
#include "FlowingFFTSpectre.h"

class ReceiverLogicNew {
private:
	Config* config;

	//Позиция приёма на текущем спектре
	float position = 0;

	float spectreWidth = 0;

	float selectedBin = 0;

	float delta = 0;

	ViewModel* viewModel;

	FlowingFFTSpectre* flowingFFTSpectre;

public:

	ReceiverLogicNew(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre);

	void setPosition(float position, bool withoutDelta);

	void updateSpectreWidth(float oldSpectreWidth, float newSpectreWidth);

	void syncFreq();

	float getFilterWidthAbs(int filterWidth);

	float getSelectedFreq();

	int getPosition();
	void saveDelta(int x);

	struct ReceiveBinArea {
		int A;
		int B;
	};

	ReceiveBinArea getReceiveBinsArea(int filterWidth, int receiverMode);

	void setFreq(float freq);

};