#pragma once

#include "Env.h"
#include "Config.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class ViewModel {
	Config* config;
public:

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

	//41965664 - хулиганы ssb в центре
	//int frequency = 41965664;
	int centerFrequency = 10000000;

	struct RSPModel {
		int gain = 50;
		bool lna = false;
	} rspModel;

	//bool gainControl = false;

	double serviceField1 = 0;
	double serviceField2 = 0;

	float waterfallMin = -80;
	float waterfallMax = -60;

	float ratio = -30;
	float minDb = -100;

	bool mouseBusy = false;

	int spectreSpeed = 20;

	bool att = false;

	//ImFont* fontStandard;
	ImFont* fontMyRegular;
	ImFont* fontBigRegular;

	struct HackRFModel {
		int lnaGain = 0;
		int vgaGain = 8;
		int enableAmp = 0;
	} hackRFModel;

	float bufferAvailable = 0;

	void setBufferAvailable(int readAvailableBufferCount);

};