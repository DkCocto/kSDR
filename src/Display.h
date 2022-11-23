#pragma once;

#include "FFTSpectreHandler.h"
#include "ReceiverLogic.h"
#include "../include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ViewModel.h"
#include "KalmanFilter.h"
#include "SMeter.h"

class Display {

private:
	
	GLFWwindow* window = nullptr;

	FFTSpectreHandler* fftSpectreHandler;

	static void framebufferReSizeCallback(GLFWwindow* window, int width, int height);

	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

	static void windowSizeCallback(GLFWwindow* window, int width, int height);

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	SMeter* smeter = new SMeter(50, 50, 400, 0.2);

public:

	static Display* instance;

	ViewModel* viewModel;

	int width = 0;
	int height = 0;

	int whichMouseBtnPressed = -1;
	int isMouseBtnPressed = 0;

	double mouseX = 0;
	double mouseY = 0;

	ReceiverLogic* receiver = NULL;
	
	Config* config;

	Display(Config* config, FFTSpectreHandler* fftSH);

	int init();

	void mainLoop();

	void drawScene();

	void drawReceivedRegion();

	void drawSpectre();

	void handleActions();

	void initImGUI();

	void renderImGUIFirst();

	void showSignaldB(float* spectre);

};