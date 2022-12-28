#pragma once

#define CONFIG_FILENAME "config.xml"

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

	double startFrequency = 7100000.0;

	double srartVolume = 0.5;

	Config(int inputSamplerate);
	~Config();

	int visibleSpectreBinCount = 2048;

	//This data will be stored in config xml
	float waterfallMin = 0;
	float waterfallMax = 0;
	float spectreRatio = 0;
	float spectreMin = 0;
	int spectreSpeed = 0;
	float lastSelectedFreq = startFrequency;
	//--------------------------------------

	void load();
	void save();

private:
	void calcOutputSamplerate();
};