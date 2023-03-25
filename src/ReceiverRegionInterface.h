#pragma once

#include "Env.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SpectreWindowData.h"
#include "Config.h"
#include "ViewModel.h"
#include "ReceiverLogic.h"
#include "string"

#define GRAY IM_COL32(95, 95, 95, 255)

using namespace std;

class ReceiverRegionInterface {

private:

	Config* config = nullptr;
	ViewModel* viewModel = nullptr;

	SpectreWindowData* sWD = nullptr;

	int freqTextWidth = 100;
	int freqTextHeight = 40;
	int freqTextX = 0;
	int freqTextY = 0;

	int freqCharWidth = 25;

	int selectedDigit = -1;

public:

	ReceiverRegionInterface() {};

	ReceiverRegionInterface(SpectreWindowData* sWD, Config* config, ViewModel* viewModel);

	void drawRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic);

	int getFreqTextWidth();
	int getFreqTextHeight();
	int getFreqTextX();
	int getFreqTextY();

	void underlineDigit(ImDrawList* draw_list, int num);
	bool markDigitByMouse(ImDrawList* draw_list, ReceiverLogic* receiverLogic);

	bool isDigitSelected();

	int getSelectedDigit();

	void setupNewFreq(bool positive, ReceiverLogic* receiverLogic);
};