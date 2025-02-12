#pragma once

#include "Env.h"
#include "Config.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class ViewModel {
	Config* config;
public:

	void storeToConfig();
	void loadFromConfig();

	ViewModel(Config* config);
	~ViewModel();

	float volume;

	int receiverMode = LSB;

	int windowWidth = 0;

	int selectedBin = 0;
	int selectedFreq = 0;

	double amp = 0;

	int filterWidth;

	bool enableNotch = false;
	int notchCenterFreq = 0;

	double signalMaxdB = 0.0;

	//41965664 - хулиганы ssb в центре
	//int frequency = 41965664;
	int centerFrequency = 10000000;

	struct RSPModel {
		int gain = 50;
		bool lna = false;
	} rspModel;

	//bool gainControl = false;

	int goToFreq = 0;

	//int test = 2300000;

	double serviceField1 = 0;
	double serviceField2 = 0;

	float waterfallMin = -80;
	float waterfallMax = -60;

	struct Waterfall {
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

	float ratio = -30;
	float minDb = -100;

	bool mouseBusy = false;

	bool att = false;

	//ImFont* fontStandard;
	ImFont* fontMyRegular;
	ImFont* fontBigRegular;

	Config::HackRF hackRF;

	Config::Transmit transmit;

	struct MyTranceiverDevice {
		bool att = false;
		bool pre = false;
		bool bypass = false;
	} myTranceiverDevice;

	float bufferAvailable = 0;

	void setBufferAvailable(int readAvailableBufferCount);

	bool removeDCBias = true;
};