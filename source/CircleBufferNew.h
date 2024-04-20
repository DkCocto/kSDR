#pragma once

#include "array"
#include "DataReceiver.h"

template<typename T>
class CircleBufferNew : public DataReceiver {

private:
	T data[CIRCLE_BUF_LEN];
	T* receiverBuf;

	int size = 0;

	int writePointer = 0;
	int readPointer = 0;

	bool wroteAtLeastOnce = false;

	bool DEBUG = false;

	Config* config;

public:

	CircleBufferNew(Config* config) {
		this->config = config;
		this->size = CIRCLE_BUF_LEN;
		memset(data, 0, sizeof(T) * size);
		receiverBuf = new T[config->fftLen];
	};

	~CircleBufferNew() {
		//delete[] data;
		delete[] receiverBuf;
	}

	/// <summary>
	/// Узнать количество ячеек данных которые можно считать на данный момент времени
	/// </summary>
	/// <returns></returns>
	int available() {
		/*if (writePointer == -1) {
			return 0;
		}*/
		/*if (writePointer == 0 && readPointer == 0 && wroteAtLeastOnce) {
			return size;
		}*/
		if (writePointer >= readPointer) {
			return writePointer - readPointer;
		} else {
			return writePointer + 1 + (size - 1 - readPointer);
		}
	}

	/// <summary>
	/// Записать массив в буфер
	/// </summary>
	/// <param name="buf">Массив</param>
	/// <param name="len">Длина массива</param>
	void write(T* buf, int len) {
		if (data == nullptr) return;
		if (len > size) {
			printf("Can't write len larger than total buffer size!\n");
			return;
		}

		if (writePointer + len < size + 1) {
			std::copy(buf, buf + len, data + writePointer);

			writePointer += len;
			if (writePointer >= size) writePointer = 0;
		} else {
			int remainsDataLen = size - writePointer;
			int canBeFilledDataLen = len - remainsDataLen;

			std::copy(buf, buf + remainsDataLen, data + writePointer);
			std::copy(buf + remainsDataLen, buf + remainsDataLen + canBeFilledDataLen, data);

			writePointer = canBeFilledDataLen;
		}
	}

	/// <summary>
	/// Считать данные в массив указанной длины
	/// </summary>
	/// <param name="len">Длина данных</param>
	/// <returns></returns>
	void read(T* arr, int len) {
		int available_ = available();
		if (len > available_) {
			printf("Trying to read more than the buffer has!\n");
			memset(arr + available_, 0, sizeof(T) * (len - available_));
			len = available_;
			return;
		}

		if (readPointer < writePointer) {
			std::copy(data + readPointer, data + readPointer + len, arr);
			readPointer += len;
			if (readPointer >= size) readPointer = 0;
		} else if (readPointer > writePointer) {
			if (readPointer + len < size) {
				std::copy(data + readPointer, data + readPointer + len, arr);
				readPointer += len;
				if (readPointer >= size) readPointer = 0;
			} else {
				std::copy(data + readPointer, data + size, arr);
				std::copy(data, data + len - (size - readPointer), arr + (size - readPointer));
				readPointer = len - (size - readPointer);
			}
		}
		if (DEBUG) {
			printf(" readPointer = %d\n", readPointer);
		}
	}

	/// <summary>
	/// Считать данные длиной в fftlen в специально созданный массив и вернуть на него ссылку. Массив удаляется автоматически.
	/// </summary>
	/// <param name="len"></param>
	/// <returns></returns>
	T* read() {
		read(receiverBuf, config->fftLen);
		return receiverBuf;
	}

	/// <summary>
	/// Считать данные длиной в len в массив и вернуть его. Не забыть удалить его после использования!
	/// </summary>
	/// <param name="len"></param>
	/// <returns></returns>
	T* read(int len) {
		T* arr = new T[len];
		read(arr, len);
		return arr;
	}

	int getWritePointer() {
		return writePointer;
	}

	int getReadPointer() {
		return readPointer;
	}

	void write(float value) { }

	void reset() {
		writePointer = 0;
		readPointer = 0;
	}

};