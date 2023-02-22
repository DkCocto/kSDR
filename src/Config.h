#pragma once

#define CONFIG_FILENAME "config.xml"

#include "Device.h"
#include "stdio.h"
#include "tinyxml2/tinyxml2.h"
#include "string"

class Config {

public:

	Device* device;

	int circleBufferLen;

	int inputSamplerate;

	int inputSamplerateDivider = 1;
	int delayedInputSamplerateDivider = inputSamplerateDivider;

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

	int fftLen = 64 * 1024;
	int delayedFFTLen = fftLen;

	float fftBandwidth;

	int defaultFilterWidth = 2700;

	double startFrequency = 7100000.0;

	Config();
	~Config();

	int visibleSpectreBinCount = 2048;

	//This data will be stored in config xml
	float waterfallMin = 0;
	float waterfallMax = 0;
	float spectreRatio = 0;
	float spectreMin = 0;
	int spectreSpeed = 0;
	float lastSelectedFreq = startFrequency;
	float volume = 1;

	int filterWidth = defaultFilterWidth;
	
	int startBin = 0;
	int stopBin = fftLen / 2 - 1;

	int removeDCBias = 1;

	struct Receiver {
		int modulation = 1;
	} receiver;

	enum DeviceType {
		RSP,
		HACKRF
	} deviceType, delayedDeviceType;

	struct HackRF {
		int deviceSamplingRate = 4000000;
		int basebandFilter = 1750000;
		char rxAmp = 0;
		char lnaGain = 16;
		char vgaGain = 20;
		char txAmp = 20;
	} hackrf;

	struct RSP {
		int deviceSamplingRate = 4000000;
		int deviceDecimationFactor = 1;
		int gain = 50;
		int lna = 0;
		char api = 2;
	} rsp;

	//--------------------------------------

	void load();
	void save();

	void setDevice(int deviceID);

private:
	void calcOutputSamplerate();
};