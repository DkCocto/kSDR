#pragma once

#include "stdio.h"
#include "mutex"

class CircleBuffer {
	float* buf = 0;
	int size;

	int readPointer = -1;

	std::mutex m;

	public:
		int writePointer = -1;
		CircleBuffer(int size);
		void write(float val);
		void write(float* buf, int bufLen);
		void write(uint8_t buf[], int bufLen);
		float read();
		float* read(int len);
		int available();
		void reset();
};