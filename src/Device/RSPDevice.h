#pragma once

#include "DeviceN.h"
#include "../CircleBufferNew.h"
#include "sdrplay\sdrplay_api.h"

class RSPDevice : public DeviceN {

private:
	const bool DEBUG = true;

	sdrplay_api_DeviceT devs[6];
	sdrplay_api_DeviceT* chosenDevice = NULL;
	sdrplay_api_DeviceParamsT* deviceParams = NULL;
	sdrplay_api_CallbackFnsT cbFns;

	CircleBufferNew<short>* bufferForSpec = nullptr;
	CircleBufferNew<short>* bufferForProc = nullptr;

	static void StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);
	static void StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);

	static void EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext);

	double freq = 7100000;
	bool needToSendFreq = false;

	int gain = 40;
	unsigned char lnaState = 0;
	bool needToSendGain = false;

	int basebandFilter = 600;
	bool needToSendBasebandFilter = false;

	void sendFreqToDevice();
	void sendGainToDevice();
	void sendBasebandFilterToDevice();

	Result initResult;

public:

	RSPDevice(Config* config) : DeviceN(config) {
		bufferForSpec = new CircleBufferNew<short>(config->circleBufferLen);
		bufferForProc = new CircleBufferNew<short>(config->circleBufferLen);
	}

	~RSPDevice();

	Result start();
	void stop();

	CircleBufferNew<short>* getBufferForSpec();
	CircleBufferNew<short>* getBufferForProc();

	sdrplay_api_DeviceT* getChosenDev();

	void setFreq(double freq);
	void setGain(int gain, unsigned char lnaState);
	void setBasebandFilter(int filterWidth);

	bool isNeedToSendFreq();
	bool isNeedToSendGain();
	bool isNeedToSendFilter();

	float prepareData(short val);

};