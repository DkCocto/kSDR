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
#include "SMeter.h"

#define GRAY IM_COL32(95, 95, 95, 255)
#define RED IM_COL32(255, 0, 0, 255)
#define BASE_COLOR IM_COL32(10, 10, 10, 100)

using namespace std;

class ReceiverRegionInterface {

private:

	Config* config = nullptr;
	ViewModel* viewModel = nullptr;

	SpectreWindowData* sWD = nullptr;

	unique_ptr<SMeter> smeter;

	const int X = 80;
	const int Y = 50;

	const int freqTextWidth = 100;
	const int freqTextHeight = 40;

	//selected approximately based on the width of one character
	const int freqCharWidth = 22;

	int backgroundX = 0;
	int backgroundY = 0;

	//16 digits X.XXX.XXX.XXX Hz
	const int backgroundWidth = 16 * freqCharWidth + 55;
	const int backgroundHeight = 150;

	const int smetreMargin = 40;

	const int backgroundPadding = 15;

	const int freqTextMarginLeft = smetreMargin;

	int freqTextX = 0;
	int freqTextY = 0;

	int selectedDigit = -1;

	void drawBackground(ImDrawList* draw_list);
	void drawReceiveRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic);

public:

	//ReceiverRegionInterface() {};

	ReceiverRegionInterface(SpectreWindowData* sWD, Config* config, ViewModel* viewModel);

	void drawRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic, FFTData::OUTPUT* spectreData);

	int getFreqTextWidth();
	int getFreqTextHeight();
	int getFreqTextX();
	int getFreqTextY();

	//void underlineDigit(ImDrawList* draw_list, int num);
	bool markDigitByMouse(ImDrawList* draw_list, ReceiverLogic* receiverLogic);

	bool isDigitSelected();

	int getSelectedDigit();

	void setupNewFreq(bool positive, ReceiverLogic* receiverLogic);
};