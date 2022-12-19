#pragma once

class Config {

public:

	int circleBufferLen;

	int inputSamplerate;

	int inputSamplerateDivider;

	int outputSamplerate;

	int outputSamplerateDivider;

	int inputChannelNumber;
	int outputChannelNumber;

	//размер буффера записи данных в звуковую карту
	int bufferWriteAudioLen;

	//размер считывания из буффера обработки данных
	int readSoundProcessorBufferLen;

	//размер записи в буффер обработки сигнала
	int audioWriteFrameLen;

	int audioReadFrameLen;

	int readSize;

	int hilbertTransformLen;
	int polyphaseFilterLen;

	int fftLen;
	float fftBandwidth;

	int defaultFilterWidth = 2700;

	double startFrequency;

	double srartVolume = 0.5;

	Config(int inputSamplerate);

	int visibleSpectreBinCount = 1024;


private:
	void calcOutputSamplerate();
};