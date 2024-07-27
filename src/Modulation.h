#pragma once

#include "Env.h"
#include "CircleBufferNew.h"

class Modulation {

private:

protected:
	int freq = 0;
	
	// = HACKRF TX BUFFER
	const int outputBufferLen = 262144;
	const int halfOutputBufferLen = 262144 / 2;

public:

	struct DataStruct {
		float* data;
		int len;
	};

	virtual void setFreq(int freq) = 0;
	int getFreq();

	virtual DataStruct* processData(CircleBufferNew<float>* buffer, int maxBufLen) = 0;
};