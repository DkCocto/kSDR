#pragma once

#include "Env.h"
#include "CircleBufferNew.h"

class Modulation {

private:


protected:
	int freq = 0;
	const int outputBufferLen = 262144;
	const int halfOutputBufferLen = 262144 >> 1;

public:
	virtual void setFreq(int freq) = 0;
	int getFreq();

	virtual Signal* processData(CircleBufferNew<float>* buffer) = 0;
};