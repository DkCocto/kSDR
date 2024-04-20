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

	double signalMaxdB = 0.0;

	//41965664 - �������� ssb � ������
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
		int enableAmp = 0;
		int basebandFilter = 1750000;
	} hackRFModel;

	float bufferAvailable = 0;

	void setBufferAvailable(int readAvailableBufferCount);

	bool removeDCBias = true;
};