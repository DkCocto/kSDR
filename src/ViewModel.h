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

	float ratio = -30;
	float minDb = -100;

	bool mouseBusy = false;

	bool att = false;

	//ImFont* fontStandard;
	ImFont* fontMyRegular;
	ImFont* fontBigRegular;

	struct HackRFModel {
		int lnaGain = 0;
		int vgaGain = 8;
		int txVgaGain = 20;
		int enableRxAmp = 0;
		int enableTxAmp = 0;
		int basebandFilter = 1750000;
	} hackRFModel;

	struct Transmit {
		bool txBySpaceBtn = false;
		float inputLevel = 1.0f;
		float inputLevel2 = 1.0f;
		float inputLevel3 = 1.0f;
		float amModulationDepth = 1.0f;
	} transmit;

	struct MyTranceiverDevice {
		bool att = false;
		bool pre = false;
		bool bypass = false;
	} myTranceiverDevice;

	float bufferAvailable = 0;

	void setBufferAvailable(int readAvailableBufferCount);

	bool removeDCBias = true;
};