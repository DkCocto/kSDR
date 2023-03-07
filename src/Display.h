#pragma once

#include "Hackrf.h"
#include "FlowingFFTSpectre.h"
#include "ReceiverLogic.h"
#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"
#include "ViewModel.h"
#include "KalmanFilter.h"
#include "SMeter.h"
#include "Spectre.h"
#include "CPU.h"
#include "RSPv2.h"
#include "ListSetting.h"
#include "functional"
#include "map"
#include "RTLDevice.h"
#include "MemoryRecordUserInterface.h"

#include <stb/stb_image.h>

class Display {

private:
	
	GLFWwindow* window = nullptr;

	FlowingFFTSpectre* flowingFFTSpectre;

	static void framebufferReSizeCallback(GLFWwindow* window, int width, int height);

	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

	static void windowSizeCallback(GLFWwindow* window, int width, int height);

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	SMeter* smeter = new SMeter(50, 50, 400, 0.2);

	CPU cpu;

	void showSelectDeviceSetting();
	void showHackrfSamplingRateSetting();


	std::unique_ptr<ListSetting> decimationLS;
	std::unique_ptr<ListSetting> hackRFsampRateLS;
	std::unique_ptr<ListSetting> rspSampRateLS;
	std::unique_ptr<ListSetting> rspDecimationFactorLS;
	std::unique_ptr<ListSetting> fftLenLS;
	std::unique_ptr<ListSetting> waterfallSpeedLS;
	std::unique_ptr<ListSetting> smoothingDepthLS;
	std::unique_ptr<ListSetting> spectreStyleLS;
	std::unique_ptr<ListSetting> rtlDeviceGainLS;
	std::unique_ptr<ListSetting> rtlSampRateLS;

	void initSettings();

	void showColorPicker(string title, unsigned int* configVal, bool withTransparency);

	MemoryRecordUserInterface memoryRecordUserInterface;

public:

	static Display* instance;

	ViewModel* viewModel;

	Spectre* spectre;

	int width = 0;
	int height = 0;

	int whichMouseBtnPressed = -1;
	int isMouseBtnPressed = 0;

	double mouseX = 0;
	double mouseY = 0;

	ReceiverLogic* receiver = NULL;
	
	Config* config;


	Display(Config* config, FFTSpectreHandler* fftSH);
	~Display();

	int init();

	void mainLoop();

	void initImGUI();

	void renderImGUIFirst();

	void showAlertOKDialog(std::string title, std::string msg);

};