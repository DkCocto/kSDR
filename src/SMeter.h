#pragma once

#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "string"
#include "ViewModel.h"

class SMeter {

	int X, Y;
	int width, height;

	ViewModel* viewModel;

	void drawGrid(ImDrawList* draw_list);
	void drawLevel(ImDrawList* draw_list, double dBValue);

	double fromdBToLevel(double dBValue);

	const char* getLevelDecodedString(double dBValue);

public:
	
	SMeter(ViewModel* viewModel);

	void update(int X, int Y, int width, int height);
	void draw(ImDrawList* draw_list, double dBValue);

};