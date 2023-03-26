#pragma once

#include "stdio.h"
#include "mutex"
#include "../DataReceiver.h"

class CircleBuffer: public DataReceiver {
	float* buf = 0;
	int size;

	int readPointer = -1;
	int writePointer = -1;

	public:
		CircleBuffer(int size);
		void write(float val);
		void write(float buf[], int bufLen);
		void write(uint8_t buf[], int bufLen);
		float read();
		float* read(int len);
		void read(float* data, int len);
		int available();
		void reset();
};