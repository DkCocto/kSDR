#include "CircleBuffer.h"
#include "chrono"
#include "thread"
#include "Delay.h"

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

void CircleBuffer::write(uint8_t* buf, int bufLen) {
	for (int i = 0; i < bufLen - 1; i++) {
		//printf("%f\r\n", (buf[i] - 127.5) / 128.0);
		//write(buf[i] * (1.f / 255.f));

		//write((float)(buf[i] - 1) / 127.0);

		//write(((buf[i] - 128.0f) / 128.0f));

		if (i % 4 == 0) {
			write(0.01 * ((buf[i] - 127.0f) / 128.0f));
			write(0.01 * ((buf[i + 1] - 127.0f) / 128.0f));
		}
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
	int countDone = 0;
	float* b = new float[len];

	for (int i = 0; i < len; i++) {
		b[i] = read();
	}
	return b;
}

void CircleBuffer::read(float* data, int len) {
	int countDone = 0;

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
	writePointer = -1;
	readPointer = -1;
}
