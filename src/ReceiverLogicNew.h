#pragma once

#include "Env.h"
#include "math.h"
#include "Config.h"

class ReceiverLogicNew {
private:
	Config* config;

	//Позиция приёма на текущем спектре
	float position = 0;

	float spectreWidth = 0;

	float totalBin = 0;
	float selectedBin = 0;

	int delta = 0;

public:

	ReceiverLogicNew(Config* config);

	void setPosition(float position, float spectreWidth);

	void update(float oldSpectreWidth, float newSpectreWidth);

	float getFilterWidthAbs(int filterWidth);

	int getSelectedFreq();

	float getSelectedBin();

	int getPosition();
	void saveDelta(int x);

	struct ReceiveBinArea {
		int A;
		int B;
	};

	ReceiveBinArea getReceiveBinsArea(int filterWidth, int receiverMode);


};