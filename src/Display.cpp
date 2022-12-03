#include "Display.h"

void Display::framebufferReSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	if (Display::instance != NULL) {
		Display::instance->width = width;
		Display::instance->height = height;
		//Display::instance->receiver->initAfterResize(width);
		Display::instance->drawScene();
		//cout << Display::display->width << " " << Display::display->height << "\r\n";
	}
}

void Display::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (Display::instance != NULL) {
		//cout << xpos << " " << ypos << "\r\n";
		//cout << Display::display->io.WantCaptureMouse;
		auto& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) {
			Display::instance->mouseX = xpos;
			Display::instance->mouseY = ypos;
		}
	}
}

void Display::windowSizeCallback(GLFWwindow* window, int width, int height) {
	framebufferReSizeCallback(window, width, height);
}

void Display::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (Display::instance != NULL) {
		auto& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) {
			Display::instance->whichMouseBtnPressed = button;
			Display::instance->isMouseBtnPressed = action;

			//if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) Display::instance->receiver->storeDeltaXPos(Display::instance->mouseX);
		}
	}
}

Display::Display(Config* config, FFTSpectreHandler* fftSH) {
	this->config = config;
	viewModel = new ViewModel(config);
	this->flowingFFTSpectre = new FlowingFFTSpectre(config, viewModel, fftSH);
	spectre = new Spectre(config, viewModel, flowingFFTSpectre);
	//flowingFFTSpectre->printCurrentPos();
	//flowingFFTSpectre->zoomIn(1500);
	//flowingFFTSpectre->printCurrentPos();
}

int Display::init() {
			
	glfwInit();

	glfwSetTime(0);

	// Create a GLFWwindow object
	window = glfwCreateWindow(1920, 1080, "kSDR", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW window\r\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(1);

	//receiver = new ReceiverLogic(config, width);

	glfwSetFramebufferSizeCallback(window, framebufferReSizeCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	initImGUI();

	glfwSwapBuffers(window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LINE_SMOOTH);

	glLineWidth(1.5);

	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1;

	//Инициализируем шрифт
}

void Display::mainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//fftSpectreHandler->getSemaphore()->lock();
		renderImGUIFirst();
		//fftSpectreHandler->getSemaphore()->unlock();
		//drawScene();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}

	glfwTerminate();
}

void Display::drawScene() {
	//glEnable(GL_POINT_SMOOTH);

	//glPointSize(2.0f); // размер точки

	//glBegin(GL_POINTS);
	//glVertex2f(0.5f, 0.5f);
	//glVertex2f(0.2f, 0.2f);
	//glEnd();

	//drawSpectre();
	//handleActions();
	//drawReceivedRegion();
}

/*void Display::drawReceivedRegion() {
	float pos = receiver->receiverPosByCoords;

	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
		glVertex2f(pos, 1.0);
		glVertex2f(pos, -1.0);
	glEnd();
	
	glColor4d(4.0 / 255, 255.0/255.0, 0, 0.2);
	glBegin(GL_QUADS);
		float delta = receiver->getFilterWidthAbs(viewModel->filterWidth);

		switch (viewModel->receiverMode) {
			case USB:
				glVertex2f(pos + delta, -1.0);
				glVertex2f(pos + delta, 1.0);
				glVertex2f(pos, 1.0);
				glVertex2f(pos, -1.0);
				break;
			case LSB:
				glVertex2f(pos - delta, -1.0);
				glVertex2f(pos - delta, 1.0);
				glVertex2f(pos, 1.0);
				glVertex2f(pos, -1.0);
				break;
			case AM:
				glVertex2f(pos - delta, -1.0);
				glVertex2f(pos - delta, 1.0);
				glVertex2f(pos + delta, 1.0);
				glVertex2f(pos + delta, -1.0);
				break;
		}

	glEnd();
}*/

//float* pipka;

/*void Display::drawSpectre() {
	glColor3d(1, 1, 1);

	//Чертим шкалу спектра
	int startPoint = 0;
	int width = receiver->windowWidth;
	int height = 200;

	//--------------------

	float* spectre = fftSpectreHandler->getOutput();

	int spectreSize = config->fftLen / 2;

	float stepX = 2.0 / (spectreSize);

	glBegin(GL_LINES);
	for (int i = 0; i < spectreSize - 1; i++) {
		glVertex2f(-1.0 + (i * stepX), spectre[fftSpectreHandler->getTrueBin(i)] / 130.0 + 0.5);
		glVertex2f(-1.0 + ((i + 1.0) * stepX), spectre[fftSpectreHandler->getTrueBin(i + 1.0)] / 130.0 + 0.5);
	}
	glEnd();

	showSignaldB(spectre);

	fftSpectreHandler->getSemaphore()->unlock();
}*/



/*void Display::showSignaldB(float* spectreData) {
	ReceiverLogicNew::ReceiveBinArea r = spectre->receiverLogicNew->getReceiveBinsArea(viewModel->filterWidth, viewModel->receiverMode);

	//viewModel->serviceField1 = r.A;

	float sum = 0.0;
	int len = r.B - r.A;

	if (len > 0) {
		//Utils::printArray(spectre, 64);
		//printf("%i %i\r\n", r.A, r.B);

		float max = -1000.0;

		for (int i = r.A; i < r.B; i++) {
			if (spectreData[fftSpectreHandler->getTrueBin(i)] > max) {
				max = spectreData[fftSpectreHandler->getTrueBin(i)];
			}
			//sum += spectre[i];
		}
		viewModel->signalMaxdB = maxdBKalman.filter(max);
	}
}*/

/*void Display::handleActions() {
	if (whichMouseBtnPressed == GLFW_MOUSE_BUTTON_1 && isMouseBtnPressed == GLFW_PRESS) {
		if (mouseX >= 0 && mouseX <= width) {
			receiver->setAbsoluteXpos(width, mouseX);
		}
	}
}*/

void Display::initImGUI() {
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO(); (void)io;

	//ImFont* fontStandard = io.Fonts->AddFontDefault();
	viewModel->fontMyRegular = io.Fonts->AddFontFromFileTTF("DroidSansMono.ttf", 18);
	viewModel->fontBigRegular = io.Fonts->AddFontFromFileTTF("DroidSansMono.ttf", 42);

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Display::renderImGUIFirst() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Проверка которая исключает настройку приемника при перетягивании стороннего окна в область окна спектра
	if (ImGui::IsMouseDown(0)) {
		if (!spectre->isMouseOnSpectreRegion(
			spectre->windowFrame.UPPER_RIGHT.x,
			spectre->windowFrame.UPPER_RIGHT.y,
			spectre->windowFrame.BOTTOM_LEFT.x,
			spectre->windowFrame.BOTTOM_LEFT.y)) viewModel->mouseBusy = true;
	} else {
		viewModel->mouseBusy = false;
	}

	ImGui::Begin(APP_NAME);                          // Create a window called "Hello, world!" and append into it.

		ImGui::SliderInt("Frequency", &viewModel->centerFrequency, 1000000, 30000000);

		ImGui::SliderInt("Filter width", &viewModel->filterWidth, 0, 12000);

		ImGui::SliderInt("Gain", &viewModel->gain, 0, 200); ImGui::SameLine();
		ImGui::Checkbox("Gain Control", &viewModel->gainControl);

		//ImGui::SameLine();
		//ImGui::Checkbox("ATT", &viewModel->att);

		ImGui::SliderFloat("Waterfall min", &viewModel->waterfallMin, -130, 0);
		ImGui::SliderFloat("Waterfall max", &viewModel->waterfallMax, -130, 0);

		ImGui::SliderFloat("Spectre ratio", &viewModel->maxDb, -150, 0);
		ImGui::SliderFloat("Spectre min val", &viewModel->minDb, -150, 0);

		ImGui::SliderInt("Spectre speed", &viewModel->spectreSpeed, 1, 200);
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//if (ImGui::Button("7100000")) spectre->receiverLogicNew->setFreq(7100000);

	ImGui::End();

	spectre->draw();

	smeter->draw(viewModel->signalMaxdB);

	ImGui::Begin("DATA");
		//ImGui::Text("stepX: %f", receiver->stepX);
		//ImGui::Text("receiverPos: %f", receiver->receiverPos);
		//ImGui::Text("selectedBin: %i", (int)round(spectre->receiverLogicNew->getSelectedBin()));
		//ImGui::Text("selectedFreq: %i", spectre->receiverLogicNew->getSelectedFreq());
		ImGui::Text("AMP: %.2f", viewModel->amp);
		ImGui::Text("CPU usage: %.1f", cpu.getCurrentValue());
		ImGui::Text("Set gain: %d", viewModel->gainFromDevice);
		//printf("%d \r\n", viewModel->gainFromDevice);
		ImGui::Text("Service field1: %f", viewModel->serviceField1);
		ImGui::Text("Service field2: %f", viewModel->serviceField2);
	ImGui::End();

	ImGui::Begin("MODE");
		if (ImGui::Button("160m")) {
			viewModel->centerFrequency = 1900000;
		}
		ImGui::SameLine();
		if (ImGui::Button("80m")) {
			viewModel->centerFrequency = 3700000;
		}
		ImGui::SameLine();
		if (ImGui::Button("40m")) {
			viewModel->centerFrequency = 7100000;
		}
		ImGui::SameLine();
		if (ImGui::Button("30m")) {
			viewModel->centerFrequency = 10325000;
		}
		ImGui::SameLine();
		if (ImGui::Button("20m")) {
			viewModel->centerFrequency = 14150000;
		}

		if (ImGui::Button("17m")) {
			viewModel->centerFrequency = 18100000;
		}
		ImGui::SameLine();
		if (ImGui::Button("15m")) {
			viewModel->centerFrequency = 21100000;
		}
		ImGui::SameLine();
		if (ImGui::Button("12m")) {
			viewModel->centerFrequency = 24900000;
		}
		ImGui::SameLine();
		if (ImGui::Button("10m")) {
			viewModel->centerFrequency = 28500000;
		}

		ImGui::RadioButton("USB", &viewModel->receiverMode, USB); ImGui::SameLine();
		ImGui::RadioButton("LSB", &viewModel->receiverMode, LSB); ImGui::SameLine();
		ImGui::RadioButton("AM", &viewModel->receiverMode, AM);

		if (ImGui::Button("100")) {
			viewModel->filterWidth = 100;
		}
		ImGui::SameLine();
		if (ImGui::Button("500")) {
			viewModel->filterWidth = 500;
		}
		ImGui::SameLine();
		if (ImGui::Button("1.2k")) {
			viewModel->filterWidth = 1200;
		}
		ImGui::SameLine();
		if (ImGui::Button("2.7k")) {
			viewModel->filterWidth = 2700;
		}
		ImGui::SameLine();
		if (ImGui::Button("3.2k")) {
			viewModel->filterWidth = 3200;
		}
		ImGui::SameLine();
		if (ImGui::Button("6.0k")) {
			viewModel->filterWidth = 6000;
		}
		ImGui::SameLine();
		if (ImGui::Button("12.0k")) {
			viewModel->filterWidth = 12000;
		}
		ImGui::SliderFloat("Volume", &viewModel->volume, 0, 5);

		if (ImGui::Button("+")) {
			flowingFFTSpectre->zoomIn(100);
			spectre->receiverLogicNew->syncFreq();
		} ImGui::SameLine();
		if (ImGui::Button("-")) {
			flowingFFTSpectre->zoomOut(100);
			spectre->receiverLogicNew->syncFreq();
		} ImGui::SameLine();
		if (ImGui::Button("<-")) {
			flowingFFTSpectre->move(-100);
			spectre->receiverLogicNew->syncFreq();
		} ImGui::SameLine();
		if (ImGui::Button("->")) {
			flowingFFTSpectre->move(100);
			spectre->receiverLogicNew->syncFreq();
		}

	ImGui::End();
}