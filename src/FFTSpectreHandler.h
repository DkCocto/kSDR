#pragma once

#include "Env.h"
#include "Config.h"
#include "Semaphore.h"
#include "WindowBlackman.h"
#include "Average.h"

class FFTSpectreHandler {
	
private:

	Config* config;

	//static const int spectreBufferLen = FFT_LENGTH;

	//Инициализируем оконный массив с размерностью длины массива буфера спектра (возвращается массив длиной +1)
	WindowBlackman* wb;
	float* windowArray;

	float* dataBuffer;

	int savedBufferPos = -1;

	int complexLen;

	float* realInput;
	float* imInput;

	float* realOut;
	float* imOut;

	//float output[complexLen * 2];
	
	float* superOutput;

	Semaphore* semaphore = new Semaphore();

	bool firstRun = true;

	int spectreSpeed = 30;

	//Average average = Average(20);

	void processFFT();
	float average(float avg, float new_sample, int n);
	void dataPostprocess();
	float* getOutput();

	int spectreSize = 0;
public:

	FFTSpectreHandler(Config* config);
	Semaphore* getSemaphore();
	float* getOutputCopy(int startPos, int len);
	bool putData(float* pieceOfData, int len);
	float psd(float re, float im);
	void prepareData();
	//int getTrueBin(int bin);
	int getSpectreSize();
	void setSpectreSpeed(int speed);

};