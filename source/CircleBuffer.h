#pragma once

#include "stdio.h"
#include "mutex"
#include "DataReceiver.h"

class CircleBuffer: public DataReceiver {
	float* buf = 0;
	int size;

	int readPointer = -1;
	int writePointer = -1;

	public:
		CircleBuffer(int size) {
			buf = new float[size];
			this->size = size;
		}

		void write(float val) {
			writePointer++;
			if (writePointer > size - 1) writePointer = 0;
			buf[writePointer] = val;
		}

		void write(float buf[], int bufLen) {
			for (int i = 0; i < bufLen; i++) {
				write(buf[i]);
			}
		}

		float read() {
			if (readPointer == writePointer) {
				printf("Nothing to read! Buffer is empty.");
			}
			readPointer++;
			if (readPointer > size - 1) readPointer = 0;
			return buf[readPointer];
		}


		float* read(int len) {
			float* b = new float[len];

			for (int i = 0; i < len; i++) {
				b[i] = read();
			}
			return b;
		}

		void read(float* data, int len) {
			for (int i = 0; i < len; i++) {
				data[i] = read();
			}
		}

		int available() {
			if (writePointer == -1) {
				return 0;
			}
			if (writePointer >= 0 && readPointer < 0) {
				return writePointer + 1;
			}
			if (writePointer >= readPointer) {
				return writePointer - readPointer;
			}
			else {
				return writePointer + 1 + (size - 1 - readPointer);
			}
		}

		void reset() {
			//printf("Buffer reset...\r\n");
			writePointer = -1;
			readPointer = -1;
		}

		void writeUint(uint8_t* buf, int len) {};
		uint8_t* readUint(int len) { return 0; };
};