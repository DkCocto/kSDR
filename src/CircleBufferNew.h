#pragma once

#include "array"
#include "DataReceiver.h"

template<typename T>
class CircleBufferNew : public DataReceiver {

private:
	T* data = nullptr;

	int size = 0;

	int writePointer = 0;
	int readPointer = 0;

	bool wroteAtLeastOnce = false;

	bool DEBUG = false;

public:

	CircleBufferNew() {};

	CircleBufferNew(int size) {
		this->size = size;
		data = new T[size];
		memset(data, 0, sizeof(T) * size);
	};

	~CircleBufferNew() {
		delete[] data;
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

	/// <summary>
	/// Считать данные в массив указанной длины
	/// </summary>
	/// <param name="len">Длина данных</param>
	/// <returns></returns>
	void read(T* arr, int len) {
		//T* arr = new T[len];
		int available_ = available();
		if (len > available_) {
			printf("Trying to read more than the buffer has!\n");
			//memset(arr + available_, 0, sizeof(T) * (len - available_));
			//len = available_;
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
		//return arr;
	}

	/// <summary>
	/// Считадь данные длиной в len в массив и вернуть его. Не забыть удалить его после использования!
	/// </summary>
	/// <param name="len"></param>
	/// <returns></returns>
	T* read(int len) {
		T* arr = new T[len];
		read(arr, len);
		return arr;
	}

	void print() {
		for (int i = 0; i < size; i++) printf("%d ", data[i]);
	}

	void printData(T* buf, int len) {
		for (int i = 0; i < len; i++) printf("%d ", buf[i]);
		printf(" -> ");
	}

	int getWritePointer() {
		return writePointer;
	}

	int getReadPointer() {
		return readPointer;
	}

	/*static bool test2() {
		int* pipika = new int[10];
		for (int i = 0; i < 10; i++) pipika[i] = i + 1;

		CircleBufferNew<int> cbn(10);
		cbn.write(pipika, 7);

		cbn.read(7);
		cbn.write(pipika, 6);

		int* data = cbn.read(7);
		Utils::printArray(data, 7);
		return true;
	}*/

	/*static bool test1() {
		int* pipika = new int[5];
		for (int i = 0; i < 5; i++) pipika[i] = i + 1;

		CircleBufferNew<int> cbn(5);
		cbn.write(pipika, 3);
		//wp = 3, rp = 0

		if (cbn.available() != 3) { printf("LABEL1\n"); return false; };

		int* data = cbn.read(2);
		if (data[0] != 1 || data[1] != 2) { printf("LABEL2\n"); return false; };
		//wp = 3, rp = 2

		data = cbn.read(1);
		if (data[0] != 3) { printf("LABEL3\n"); return false; };
		//wp = 3, rp = 3

		cbn.write(pipika, 4);
		//wp = 2, rp = 3

		data = cbn.read(2);
		if (data[0] != 1 || data[1] != 2) { printf("LABEL4\n"); return false; };
		//wp = 2, rp = 0

		data = cbn.read(2);
		if (data[0] != 3 || data[1] != 4) { printf("LABEL5\n"); return false; };
		//wp = 2, rp = 2

		cbn.print();
		printf("writePointer = %d, readPointer = %d\n", cbn.getWritePointer(), cbn.getReadPointer());
		//Utils::printArray(data, 1);

		return true;
	}*/

	void write(float value) {

	}
	void reset() {
		writePointer = 0;
		readPointer = 0;
	}

};