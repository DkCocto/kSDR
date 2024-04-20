#pragma once

#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "string"
#include "ViewModel.h"
#include "Spectre/FFTData.h"
#include "ReceiverLogic.h"
#include "Average.h"

class SMeter {

	int X, Y;
	int width, height;

	ViewModel* viewModel;

	void drawGrid(ImDrawList* draw_list);
	void drawLevel(ImDrawList* draw_list, FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic);

	float fromdBToLevel(float dBValue);

	float fromLevelToDb(float level);

	const char* getLevelDecodedString(float dBValue);

	Average averageSignalDb;

public:
	
	SMeter(ViewModel* viewModel);

	void update(int X, int Y, int width, int height);
	void draw(ImDrawList* draw_list, FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic);

	float getSignaldB(FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic);

};