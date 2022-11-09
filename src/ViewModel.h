#pragma once

#include "Env.h"

class ViewModel {
public:

	float volume = 1;

	int receiverMode = USB;

	int windowWidth = 0;

	float absoluteXpos = 0, stepX = 0, receiverPos = 0;

	int selectedBin = 0;
	int selectedFreq = 0;

	double amp = 0;

	int filterWidth = 3000;

	//41965664 - хулиганы ssb в центре
	//int frequency = 41965664;
	int frequency = 3700000;


	int gain = 320;
};