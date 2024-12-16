#pragma once

#include "src/FastFir/jfastfir.h"

template<typename T>
class Decimator {
private:

	int count = 0;
	int inputDataLen = 0;
	int divider = 0;
	int deviceSampleRate = 0;

	vector<double> arrI;
	vector<double> arrQ;

	JFastFIRFilter* firI;
	JFastFIRFilter* firQ;

public:

	Decimator(int divider, int deviceSampleRate, int inputDataLen) {
		this.inputDataLen = inputDataLen;
		this->divider = divider;
		this->deviceSampleRate = deviceSampleRate;

		arrI.reserve(inputDataLen / 2);
		arrQ.reserve(inputDataLen / 2);

		firI = new JFastFIRFilter;
		firQ = new JFastFIRFilter;

		int workingSampleRate = deviceSampleRate / divider;

		firI->setKernel(JFilterDesign::BandPassHanning(
			workingSampleRate / 2,
			deviceSampleRate,
			64)
		);

		firQ->setKernel(JFilterDesign::BandPassHanning(
			workingSampleRate / 2,
			deviceSampleRate,
			64)
		);
	}

	~Decimator() {
		delete[] arrI;
		delete[] arrQ;

		delete arrI;
		delete arrQ;
	}


	void put(T* srcArray, int srcArrayLen, T* resArray) {
		count = 0;

		for (int i = 0; i < srcArrayLen / 2; i++) {
			arrI[i] = (double)srcArray[2 * i];
			arrQ[i] = (double)srcArray[2 * i + 1];
		}

		firI->Update(arrI);
		firQ->Update(arrQ);

		for (int i = 0; i < srcArrayLen / 2; i++) {
			if (i % divider == 0) {
				resArray[2 * count] = (T)arrI[i];
				resArray[2 * count + 1] = (T)arrQ[i];
				count++;
			}
		}

	}

};