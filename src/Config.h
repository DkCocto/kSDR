#pragma once

constexpr auto CONFIG_FILENAME = "config.xml";

#include "Env.h"
#include "Device.h"
#include "stdio.h"
#include "tinyxml2/tinyxml2.h"
#include "string"
#include "vector"
#include "memory"

#define CIRCLE_BUF_LEN 2 * 1024 * 1024

class Config {

public:

	//global flags
	std::atomic_bool WORKING = true;
	bool TRANSMITTING = false;
	//------------

	//int circleBufferLen;

	int currentWorkingInputSamplerate;

	int inputSamplerateDivider = 1;
	int delayedInputSamplerateDivider = inputSamplerateDivider;

	int inputSamplerateSound;

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

	int HILBERT_TRANSFORM_LEN;
	int polyphaseFilterLen;

	int fftLen = 64 * 1024;
	int delayedFFTLen = fftLen;

	float fftBandwidth;

	int defaultFilterWidth = 2700;

	double startFrequency = 7100000.0;

	Config();
	~Config();

	int visibleSpectreBinCount = 4096;

	//This data will be stored in config xml
	float waterfallMin = 0;
	float waterfallMax = 0;
	float spectreRatio = 0;
	float spectreMin = 0;
	float lastSelectedFreq = (float)startFrequency;
	float volume = 1;

	int filterWidth = defaultFilterWidth;
	
	int startBin = 0;
	int stopBin = fftLen / 2 - 1;

	int removeDCBias = 1;

	struct Waterfall {
		int speed = 1;
		float value1 = 0;
		float value2 = 0;
		float value3 = 0;
		float value4 = 0;
		float value5 = 0;
		float value6 = 0;
		float value7 = 0;
		float value8 = 0;
		float value9 = 0;
		float value10 = 0;
	} waterfall;

	struct MemoryRecord {
		std::string desc;
		float freq;
		int modulation = 1;
		int filterWidth;
	};

	std::vector<MemoryRecord> memoryVector;

	struct Spectre {
		int style = 0;
		bool contourShowsPower = 0;
		float bottomCoeff = 1;
		float topCoeff = 1;
		bool hangAndDecay = false;
		float decaySpeed = 0.1f;
		float decaySpeedDelta = 1.01f;
		int spectreSpeed = 8;
		int spectreSpeed2 = 5;
		int smoothingDepth = 2;
		int spectreCorrectionDb = -50;
		int visibleStartFreq = 7000000;
		int visibleStopFreq = 7200000;
		int minDb = 0;
		int maxDb = 0;
		int averageDb = 0;
		int minVisibleDB = 0;
		int maxVisibleDB = 0;
	} spectre;

	struct Receiver {
		int modulation = 1;
		int frequencyShift = 0;
		bool enableFrequencyShift = 0;

		struct AGC {
			double threshold = 0.01;
			double atackSpeedMs = 1.0;
			double holdingTimeMs = 100;
			double releaseSpeed = 0.00005;
			double lastAmp = 1;
		} agc;

		bool enableNotch = 0;
		int notchCenterFreq = 0;
		int notchWidth = 400;
		int smeterType = 0;

		int receiveBinA = 0;
		int receiveBinB = 0;
	} receiver;

	struct Transmit {
		bool txBySpaceBtn = false;
		float inputLevel = 1.0f;
		float inputLevel2 = 1.0f;
		float inputLevel3 = 1.0f;
		float amModulationDepth = 1.0f;
		bool sendToneSignal = false;
		int txFreq = 0;
		int tone1Freq = 1000;
		int tone2Freq = 0;
		int outputPower = 10;
		bool phaserFilter = true;
	} transmit;

	DeviceType deviceType, delayedDeviceType;

	struct HackRF {
		int deviceSamplingRate = 4000000;
		int basebandFilter = 1750000;
		int rxAmp = 0;
		int txAmp = 0;
		int lnaGain = 16;
		int vgaGain = 20;
		int txVgaGain = 47;
	} hackrf;

	struct RSP {
		int deviceSamplingRate = 4000000;
		int deviceDecimationFactor = 1;
		int gain = 50;
		int lna = 0;
		char api = 2;
		int basebandFilter = 600000;
	} rsp;

	struct RTL {
		int deviceSamplingRate = 500000;
		int gain = 50;
	} rtl;

	struct SOUNDCARDDEVICE {
		int deviceSamplingRate = 192000;
		int inputChannelCount = 2;
		int inputFrameCount = inputChannelCount * 16;
	} soundcardDevice;

	struct App {
		int winWidth = 1920;
		int winHeight = 1080;
	} app;

	struct ColorTheme {
		unsigned int spectreWindowFillColor1 = 0xFFFFFFFF;
		unsigned int spectreWindowFillColor2 = 0xFFFFFFFF;
		unsigned int spectreFillColor = 0xFFFFFFFF;
		bool spectreGradientEnable = true;
		unsigned int spectreProfileColor = 0xFFFFFFFF;
		unsigned int mainBGColor = 0xFFFFFFFF;
		unsigned int windowsBGColor = 0xFFFFFFFF;
		unsigned int windowsTitleBGColor = 0xFFFFFFFF;
		unsigned int receiveRegionColor = 0xFFFFFFFF;
		unsigned int memoryItemSelectedColor = 4278420240;
	} colorTheme;

	//--------------------------------------

	struct MyTranceiverDevice {
		enum BAND {
			B_160,
			B_80,
			B_40,
			B_30,
			B_20,
			B_17,
			B_15,
			B_12,
			B_10,
			UNDEF
		};
		bool att = false;
		bool pre = false;
		bool bypass = false;
	} myTranceiverDevice;

	void loadXml();
	void initSettings();
	void save();
	void loadMemory();
	void saveMemory();

	void setDevice(int deviceID);

	void storeRecord(MemoryRecord memRec);
	void editRecord(MemoryRecord correctedMemRec, int index);
	void deleteRecord(int index);

	void prepareConfiguration();

private:
	void calcOutputSamplerate();
	void calcInputSamplerate();
};