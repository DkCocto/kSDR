#include "Display.h"

void Display::framebufferReSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	if (Display::instance != NULL) {
		Display::instance->width = width;
		Display::instance->height = height;
		Display::instance->receiver->initAfterResize(width);
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

			if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) Display::instance->receiver->storeDeltaXPos(Display::instance->mouseX);
		}
	}
	//cout << button << " " << action << " " << mods << "\r\n";
}

Display::Display(Config* config, FFTSpectreHandler* fftSH) {
	this->config = config;
	fftSpectreHandler = fftSH;
}

int Display::init() {

	glfwInit();

	glfwSetTime(0);

	// Create a GLFWwindow object
	window = glfwCreateWindow(1280, 1024, "kSDR", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW window\r\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(1);

	receiver = new ReceiverLogic(config, width);

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
}

void Display::mainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderImGUIFirst();

		drawScene();

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

	drawSpectre();
	handleActions();
	drawReceivedRegion();
}

void Display::drawReceivedRegion() {
	float pos = receiver->receiverPosByCoords;

	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
		glVertex2f(pos, 1.0);
		glVertex2f(pos, -1.0);
	//cout << pos << "\r\n";
	glEnd();
	
	glColor4d(4.0 / 255, 255.0/255.0, 0, 0.2);
	glBegin(GL_QUADS);
		float delta = receiver->getFilterWidthAbs(viewModel.filterWidth);

		switch (viewModel.receiverMode) {
			case USB:
				glVertex2f(pos - delta, -1.0);
				glVertex2f(pos - delta, 1.0);
				glVertex2f(pos, 1.0);
				glVertex2f(pos, -1.0);
				break;
			case LSB:
				glVertex2f(pos + delta, -1.0);
				glVertex2f(pos + delta, 1.0);
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
}

//float* pipka;

void Display::drawSpectre() {
	glColor3d(1, 1, 1);

	//Чертим шкалу спектра
	int startPoint = 0;
	int width = receiver->windowWidth;
	int height = 200;




	//--------------------

	float* pipka = fftSpectreHandler->getOutput();

	int pipkaSize = config->fftLen / 2;

	float stepX = 2.0 / (pipkaSize);

	glBegin(GL_LINES);
	for (int i = 0; i < pipkaSize - 1; i++) {
		glVertex2f(-1 + (i * stepX), pipka[fftSpectreHandler->getTrueBin(i)] + 1.5);
		glVertex2f(-1 + ((i + 1.0) * stepX), pipka[fftSpectreHandler->getTrueBin(i + 1)] + 1.5);
	}
	glEnd();
	fftSpectreHandler->getSemaphore()->unlock();
}

void Display::handleActions() {
	if (whichMouseBtnPressed == GLFW_MOUSE_BUTTON_1 && isMouseBtnPressed == GLFW_PRESS) {
		if (mouseX >= 0 && mouseX <= width) {
			receiver->setAbsoluteXpos(width, mouseX);
		}
	}
}

void Display::initImGUI() {
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Display::renderImGUIFirst() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//static float f = 0.0f;
	//static int counter = 0;

	ImGui::Begin(APP_NAME);                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("windowWidth: %i", receiver->windowWidth);
	ImGui::Text("absoluteXpos: %f", receiver->absoluteXpos);
	ImGui::Text("stepX: %f", receiver->stepX);
	ImGui::Text("receiverPos: %f", receiver->receiverPos);
	ImGui::Text("selectedBin: %i", (int)round(receiver->selectedBin));
	ImGui::Text("selectedFreq: %i", receiver->getSelectedFreq());
	ImGui::Text("Frequency: %i", viewModel.frequency + receiver->getSelectedFreq());
	ImGui::Text("AMP: %f", viewModel.amp);

	ImGui::SliderFloat("Volume", &viewModel.volume, 0, 5);
	ImGui::SliderInt("Filter width", &viewModel.filterWidth, 0, 12000);
	ImGui::SliderInt("Frequency", &viewModel.frequency, 40000000, 50000000);
	ImGui::SliderInt("Gain", &viewModel.gain, 0, 1000);

	ImGui::RadioButton("USB", &viewModel.receiverMode, USB); ImGui::SameLine();
	ImGui::RadioButton("LSB", &viewModel.receiverMode, LSB); ImGui::SameLine();
	ImGui::RadioButton("AM", &viewModel.receiverMode, AM);


	// 
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_demo_window);

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

/*float getValueRatio(data: DoubleArray, centerY : Int) : Double{
	val minValueInData = data.minOrNull()
	val maxValueInData = data.maxOrNull()

	var maxValue = 0.0

	if (minValueInData != null && maxValueInData != null) {
		val minValueInDataAbs = abs(minValueInData)
		val maxValueInDataAbs = abs(maxValueInData)
		if (minValueInDataAbs >= maxValueInDataAbs) maxValue = minValueInDataAbs
		if (minValueInDataAbs < maxValueInDataAbs) maxValue = maxValueInDataAbs
	}

	return (height - centerY - paddingBottom) / maxValue
}*/