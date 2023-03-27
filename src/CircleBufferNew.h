#pragma once

#include "array"

template<typename T>
class CircleBufferNew {

private:
	T* data = nullptr;

	int size = 0;

	int writePointer = 0;
	int readPointer = 0;

public:
	CircleBufferNew(int size) {
		this->size = size;
		data = new T[size];
		memset(data, 0, sizeof(T) * size);
	};

	int available() {
		/*if (writePointer == -1) {
			return 0;
		}
		if (writePointer >= 0 && readPointer < 0) {
			return writePointer + 1;
		}*/
		if (writePointer >= readPointer) {
			return writePointer - readPointer;
		} else {
			return writePointer + 1 + (size - 1 - readPointer);
		}
	}

	//write and read pointer это индекс массива
	void write(T* buf, int len) {

		if (len > size) {
			printf("Can't write len larger than total buffer size!\n");
			return;
		}

		if (DEBUG) printData(buf, len);

		if (writePointer + len < size + 1) {
			std::copy(buf, buf + len, data + writePointer);

			writePointer += len;
			if (writePointer >= size) writePointer = 0;
		} else {
			int remainsDataLen = size - writePointer;
			int canBeFilledDataLen = len - remainsDataLen;
			//printf("remainsDataLen = %d; canBeFilledDataLen = %d\r\n", remainsDataLen, canBeFilledDataLen);

			std::copy(buf, buf + remainsDataLen, data + writePointer);
			std::copy(buf + remainsDataLen, buf + remainsDataLen + canBeFilledDataLen, data);

			writePointer = canBeFilledDataLen;
		}

		/*if ((writePointer + 1) + (len + 1) < size) {
			
			std::copy(buf, buf + len, data + writePointer + 1);
			writePointer += len;

		} else {
			int k = (titka + len) - (size - 1);
			int a = (size - 1) - titka;
			//printf("%d\r\n", a);

			std::copy(buf, buf + a + 1, data + titka);

			std::copy(buf + a + 1, buf + a + k, data);

			writePointer = k - 2;
		}*/
		if (DEBUG) {
			print();
			printf(" writePointer = %d, readPointer = %d", writePointer, readPointer);
			printf("\n");
		}
	}

	//write and read pointer это индекс массива
	T* read(int len) {
		T* arr = new T[len];

		int available_ = available();
		if (len > available_) {
			printf("Trying to read more than the buffer has!\n");
			memset(arr + available_, 0, sizeof(T) * (len - available_));
			len = available_;
		}

		if (readPointer < writePointer) {
			std::copy(data + readPointer, data + readPointer + len, arr);
			readPointer += len;
			if (readPointer >= size) readPointer = 0;
		} else {
			std::copy(data + readPointer, data + size, arr);
			std::copy(data, data + len - (size - readPointer), arr + (size - readPointer));
			readPointer = size - readPointer - 1;
		}
		if (DEBUG) {
			printf(" readPointer = %d\n", readPointer);
		}
		return arr;
	}

	void print() {
		for (int i = 0; i < size; i++) printf("%d ", data[i]);
	}

	void printData(T* buf, int len) {
		for (int i = 0; i < len; i++) printf("%d ", buf[i]);
		printf(" -> ");
	}

};