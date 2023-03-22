#include "CircleBuffer.h"

CircleBuffer::CircleBuffer(int size) {
	buf = new float[size];
	this->size = size;
}

void CircleBuffer::write(float val) {
 	writePointer++;
	if (writePointer > size - 1) writePointer = 0;
	buf[writePointer] = val;
}

void CircleBuffer::write(float buf[], int bufLen) {
	for (int i = 0; i < bufLen; i++) {
		write(buf[i]);
	}
}

float CircleBuffer::read() {
	if (readPointer == writePointer) {
		printf("Nothing to read! Buffer is empty.");
	}
	readPointer++;
	if (readPointer > size - 1) readPointer = 0;
	return buf[readPointer];
}


float* CircleBuffer::read(int len) {
	float* b = new float[len];

	for (int i = 0; i < len; i++) {
		b[i] = read();
	}
	return b;
}

void CircleBuffer::read(float* data, int len) {
	for (int i = 0; i < len; i++) {
		data[i] = read();
	}
}

int CircleBuffer::available() {
	if (writePointer == -1) {
		return 0;
	}
	if (writePointer >= 0 && readPointer < 0) {
		return writePointer + 1;
	}
	if (writePointer >= readPointer) {
		return writePointer - readPointer;
	} else {
		return writePointer + 1 + (size - 1 - readPointer);
	}
}

void CircleBuffer::reset() {
	printf("Buffer reset...\r\n");
	writePointer = -1;
	readPointer = -1;
}
