#pragma once

#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "string"

class SMeter {

	double x, y;
	double width, height;

	void drawGrid(ImDrawList* draw_list);
	void drawLevel(ImDrawList* draw_list, double dBValue);

	double fromdBToLevel(double dBValue);

const char* getLevelDecodedString(double dBValue);

public:

	SMeter(double x, double y, double width, double height);

	void draw(double dBValue);

};