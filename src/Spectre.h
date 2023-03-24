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
#include "ColoredSpectreBG.h"
#include "ReceiverRegionInterface.h"
#include "SpectreWindowData.h"
#include "string"
#include "vector"
#include <chrono>
#include "iostream"

using namespace std;

class Spectre {

	ViewModel* viewModel;

	unique_ptr<KalmanFilter> maxdBKalman;
	unique_ptr<KalmanFilter> ratioKalman;
	unique_ptr<KalmanFilter> spectreTranferKalman;
	
	Config* config;

	ColoredSpectreBG coloredSpectreBG;

	int spectreWidth = 0;
	int spectreHeight = 0;

	bool disableControl_ = false;
	
public:

	struct MIN_MAX {
		float min;
		float max;
		float average;
	};

	struct Region {
		ImVec2 x1;
		ImVec2 x2;
	};

	void disableControl(int id);
	void enableControl(int id);

	bool isMouseOnRegion(Spectre::Region region);

private:
	MIN_MAX minMax;

	SpectreWindowData sWD;

	struct WINDOW_FRAME {
		ImVec2 UPPER_RIGHT;
		ImVec2 BOTTOM_LEFT;
	};

	Spectre::MIN_MAX getMinMaxInSpectre(std::vector<float> spectreData, int len);

	void handleEvents(int spectreWidthInPX, ReceiverLogic* receiverLogicNew, FlowingFFTSpectre* flowingSpectre);

	int getMousePosXOnSpectreWindow();

	void drawFreqMarks(ImDrawList* draw_list, ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, int spectreHeight, FlowingFFTSpectre* flowingFFTSpectre);

	void drawSpectreContour(FFTData::OUTPUT* fullSpectreData, ImDrawList* draw_list, FlowingFFTSpectre* flowingFFTSpectre);

	void drawFreqPointerMark(ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, ImDrawList* draw_list, ReceiverLogic* receiverLogicNew);

	void drawMemoryMarks(ImDrawList* draw_list, FlowingFFTSpectre* flowingFFTSpectre, ReceiverLogic* receiverLogicNew);

	ReceiverRegionInterface receiverRegionInterface;

public:

	void executeMemoryRecord(Config::MemoryRecord record, ReceiverLogic* receiverLogicNew);

	unique_ptr<Waterfall> waterfall;

	MIN_MAX getMinMaxInSpectre();

	Spectre(Config* config, ViewModel* viewModel);
	
	void draw(ReceiverLogic* receiverLogicNew, FlowingFFTSpectre* flowingFFTSpectre);

	void storeSignaldB(FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogicNew);

	WINDOW_FRAME windowFrame { ImVec2(0, 0), ImVec2(0, 0) };

	void waterfallAutoColorCorrection();
	void spectreRatioAutoCorrection();
};