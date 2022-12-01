#pragma once

#include "Env.h"
#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "FlowingFFTSpectre.h"
#include "ReceiverLogicNew.h"
#include "ViewModel.h"
#include "KalmanFilter.h"
#include "Waterfall.h"

class Spectre {

	int rightPadding = 40;
	int leftPadding = 40;
	int waterfallPaddingTop = 50;

	FlowingFFTSpectre* flowingFFTSectre;

	ViewModel* viewModel;

	KalmanFilter* maxdBKalman;
	KalmanFilter* ratioKalman;
	KalmanFilter* spectreTranferKalman;
	

	Config* config;

	struct MIN_MAX {
		float min;
		float max;
	};

	struct WINDOW_FRAME {
		ImVec2 UPPER_RIGHT;
		ImVec2 BOTTOM_LEFT;
	};

	MIN_MAX getMinMaxInSpectre(float* spectreData, int len);

	Waterfall* waterfall;

	void handleEvents(ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX);

	void drawFreqMarks(ImDrawList* draw_list, ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, int spectreHeight);

public:

	ReceiverLogicNew* receiverLogicNew;

	Spectre(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSectre);
	void draw();

	void storeSignaldB(float* spectreData);

	bool isMouseOnSpectreRegion(int spectreX1, int spectreY1, int spectreX2, int spectreY2);

	WINDOW_FRAME windowFrame{ ImVec2(0, 0), ImVec2(0, 0) };
};