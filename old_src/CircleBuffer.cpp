#include "CircleBuffer.h"
#include "chrono"
#include "thread"

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

void CircleBuffer::write(uint8_t buf[], int bufLen) {
	//printf("%i\r\n", bufLen);
	for (int i = 0; i < bufLen; i++) {
		write((buf[i] - 127.4) / 128);
	}
}

float CircleBuffer::read() {
	if (readPointer == writePointer) {
		printf("Nothing to read! Buffer is empty.");
		//throw "Nothing to read! Buffer is empty.";
	}
	readPointer++;
	if (readPointer > size - 1) readPointer = 0;
	return buf[readPointer];
}


float* CircleBuffer::read(int len) {
	int countDone = 0;
	float* b = new float[len];
	//if (available() >= len) {
	//	memcpy(b, buf + readPointer, sizeof(b) * len);
	//}
	for (int i = 0; i < len; i++) {
		b[i] = read();
	}
	return b;

	//while (true) {
	//	if (available() > 0) {
	//		b[countDone] = read();
	//		countDone++;
	//		if (countDone > len - 1) {
	//			m.unlock();
	//			return b;
	//		}
	//	}
	//}
	//m.unlock();
	//return b;
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