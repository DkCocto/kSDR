#pragma once

#include "Env.h"
#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "FFTSpectreHandler.h"
#include "ReceiverLogicNew.h"
#include "ViewModel.h"
#include "KalmanFilter.h"
#include "Waterfall.h"

class Spectre {

	double width, height;

	FFTSpectreHandler* fftSH;

	ViewModel* viewModel;

	bool isMouseOnSpectreRegion(int spectreX1, int spectreY1, int spectreX2, int spectreY2);

	KalmanFilter* maxdBKalman;
	KalmanFilter* ratioKalman;

	Config* config;

	struct MIN_MAX {
		float min;
		float max;
	};

	MIN_MAX getMinMaxInSpectre(float* spectreData, int len);

	Waterfall* waterfall;

public:

	ReceiverLogicNew* receiverLogicNew;

	Spectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH, double width, double height);
	void draw();

	void storeSignaldB(float* spectreData);

};