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
#include "Environment.h"
#include "Utils.h"

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

	const int buttonGroupWidth4Col = 250;
	const int buttonGroupWidth2Col = 105;

	int backgroundWidth = 16 * freqCharWidth + 55;
	const int backgroundHeight = 152;

	const int smetreMargin = 40;

	const int backgroundPadding = 15;

	const int freqTextMarginLeft = smetreMargin;

	int freqTextX = 0;
	int freqTextY = 0;

	int selectedDigit = -1;

	void drawBackground(ImDrawList* draw_list);
	void drawReceiveRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic);

	Environment* env;

public:

	//ReceiverRegionInterface() {};

	ReceiverRegionInterface(SpectreWindowData* sWD, Config* config, ViewModel* viewModel, Environment* env);

	void drawRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic, FFTData::OUTPUT* spectreData);

	void drawFeatureMarker(ImFont* fontMyRegular, ImDrawList* draw_list, int x, int y, ImU32 col, string msg);
	void drawFeatureMarker4withTitle(ImFont* fontMyRegular, ImDrawList* draw_list, int x, int y, ImU32 col, string title, string msg);

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