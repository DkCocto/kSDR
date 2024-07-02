#include "Display.h"

void Display::framebufferReSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	if (Display::instance != NULL) {
		Display::instance->width = width;
		Display::instance->height = height;
	}
}

void Display::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (Display::instance != NULL) {
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
		}
	}
}

Display::Display(Environment* env) {
	this->env = env;
	spectre = new Spectre(env);
	viewModel = env->getViewModel();
	memoryRecordUserInterface = MemoryRecordUserInterface(env->getConfig(), viewModel, spectre);
}

bool errorInitDeviceShowed = false;

int Display::init() {

	glfwInit();

	glfwSetTime(0);

	/*int i;
	GLFWmonitor** monitors = glfwGetMonitors(&i);

	const GLFWvidmode* mode = glfwGetVideoMode(monitors[0]);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);*/

	// Create a GLFWwindow object
	window = glfwCreateWindow(env->getConfig()->app.winWidth, env->getConfig()->app.winHeight, "kSDR", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW window\r\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebufferReSizeCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	initImGUI();

	glfwSwapBuffers(window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_LINE_SMOOTH);

	//glLineWidth(1.5);

	int channels, width, height;
	unsigned char* pixels = stbi_load("icon.png", &width, &height, &channels, 4); //rgba channels 
	
	GLFWimage images[1];
	images[0].width = width;
	images[0].height = height;
	images[0].pixels = pixels;

	glfwSetWindowIcon(window, 1, images);
	
	stbi_image_free(images[0].pixels);

	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1;

	ImGuiStyle& style = ImGui::GetStyle();
	style.AntiAliasedLines = true;
	style.IndentSpacing = 20;
	//style.AntiAliasedFill = true;

	initSettings();
}

void Display::mainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		ImVec4 bgColor = ImGui::ColorConvertU32ToFloat4(env->getConfig()->colorTheme.mainBGColor);

		glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderImGUIFirst();

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

Display::~Display() {
	//delete viewModel;
	//delete flowingFFTSpectre;
}


void Display::initImGUI() {
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO(); (void)io;

	//ImFont* fontStandard = io.Fonts->AddFontDefault();
	//viewModel->fontMyRegular = io.Fonts->AddFontFromFileTTF("DroidSansMono.ttf", 18);
	viewModel->fontMyRegular = io.Fonts->AddFontFromFileTTF("m-regular.ttf", 18);
	viewModel->fontBigRegular = io.Fonts->AddFontFromFileTTF("m-regular.ttf", 43);

	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Display::renderImGUIFirst() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (env->reloading) {
		errorInitDeviceShowed = false;
		env->reload();
		return;
	}

	//Store window size
	env->getConfig()->app.winWidth = width;
	env->getConfig()->app.winHeight = height;

	//Проверка которая исключает настройку приемника при перетягивании стороннего окна в область окна спектра
	if (ImGui::IsMouseDown(0)) {
		if (!spectre->isMouseOnRegion(
			Spectre::Region{ 
				ImVec2(spectre->windowFrame.UPPER_RIGHT.x, spectre->windowFrame.UPPER_RIGHT.y),
				ImVec2(spectre->windowFrame.BOTTOM_LEFT.x, spectre->windowFrame.BOTTOM_LEFT.y) }
			)
		) viewModel->mouseBusy = true;
	} else {
		viewModel->mouseBusy = false;
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(env->getConfig()->colorTheme.windowsBGColor));
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::ColorConvertU32ToFloat4(env->getConfig()->colorTheme.windowsTitleBGColor));

	initDynamicSettings();

	//ImGui::PushID(999);
	ImGui::Begin(APP_NAME);

		ImGui::Spacing(); ImGui::Spacing();

		ImGui::SliderFloat("Volume", &viewModel->volume, 0, 7);

		if (ImGui::Button("GO")) {
			env->getReceiverLogic()->setFreq((float)viewModel->goToFreq);
		}
		ImGui::SameLine();
		ImGui::InputInt("To freq", &viewModel->goToFreq, 500, 1000);

		if (ImGui::Button("160m")) { env->getReceiverLogic()->setFreq(1900000); viewModel->receiverMode = LSB; } ImGui::SameLine();
		if (ImGui::Button("80m")) { env->getReceiverLogic()->setFreq(3700000); viewModel->receiverMode = LSB; } ImGui::SameLine();
		if (ImGui::Button("40m")) { env->getReceiverLogic()->setFreq(7100000); viewModel->receiverMode = LSB; } ImGui::SameLine();
		if (ImGui::Button("30m")) { env->getReceiverLogic()->setFreq(10300000); viewModel->receiverMode = USB; } ImGui::SameLine();
		if (ImGui::Button("20m")) { env->getReceiverLogic()->setFreq(14150000); viewModel->receiverMode = USB; }
		if (ImGui::Button("17m")) { env->getReceiverLogic()->setFreq(18100000); viewModel->receiverMode = USB; } ImGui::SameLine();
		if (ImGui::Button("15m")) { env->getReceiverLogic()->setFreq(21100000); viewModel->receiverMode = USB; } ImGui::SameLine();
		if (ImGui::Button("12m")) { env->getReceiverLogic()->setFreq(24900000); viewModel->receiverMode = USB; } ImGui::SameLine();
		if (ImGui::Button("10m")) { env->getReceiverLogic()->setFreq(28500000); viewModel->receiverMode = USB; }

		ImGui::RadioButton("DSB", &viewModel->receiverMode, DSB); ImGui::SameLine();
		ImGui::RadioButton("USB", &viewModel->receiverMode, USB); ImGui::SameLine();
		ImGui::RadioButton("LSB", &viewModel->receiverMode, LSB); ImGui::SameLine();
		ImGui::RadioButton("AM", &viewModel->receiverMode, AM); ImGui::SameLine();
		ImGui::RadioButton("nFM", &viewModel->receiverMode, nFM);
		
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (env->getComPortHandler()->isConnected()) {
			if (ImGui::TreeNode("Tranceiver Device")) {
				ImGui::Spacing();

				ImGui::Checkbox("ATT", &viewModel->myTranceiverDevice.att);

				ImGui::Checkbox("PRE", &viewModel->myTranceiverDevice.pre);

				ImGui::Checkbox("DPF ByPass", &viewModel->myTranceiverDevice.bypass);

				ImGui::TreePop();
			}
		}

		//ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		//if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
		if (ImGui::TreeNode("Control")) {
			ImGui::Spacing();

			ImGui::SeparatorText("Receiver");

			ImGui::SliderInt("Center freq", &viewModel->centerFrequency, 1000000, 30000000);

			if (ImGui::Button("+")) {
				env->getFlowingSpectre()->zoomIn();
				env->getReceiverLogic()->setFrequencyDelta(env->getReceiverLogic()->getFrequencyDelta());
			} ImGui::SameLine();
			if (ImGui::Button("-")) {
				env->getFlowingSpectre()->zoomOut();
				env->getReceiverLogic()->setFrequencyDelta(env->getReceiverLogic()->getFrequencyDelta());
			} ImGui::SameLine();
			if (ImGui::Button("<-")) {
				env->getFlowingSpectre()->move(-250);
				env->getReceiverLogic()->setFrequencyDelta(env->getReceiverLogic()->getFrequencyDelta());
			} ImGui::SameLine();
			if (ImGui::Button("->")) {
				env->getFlowingSpectre()->move(250);
				env->getReceiverLogic()->setFrequencyDelta(env->getReceiverLogic()->getFrequencyDelta());
			}

			ImGui::Spacing();

			ImGui::SeparatorText("Audio Filter");

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

			if (ImGui::Button("3.2k")) {
				viewModel->filterWidth = 3200;
			}
			ImGui::SameLine();
			if (ImGui::Button("6.0k")) {
				viewModel->filterWidth = 6000;
			}
			ImGui::SameLine();
			if (ImGui::Button("8.0k")) {
				viewModel->filterWidth = 8000;
			}
			ImGui::SameLine();
			if (ImGui::Button("12.0k")) {
				viewModel->filterWidth = 12000;
			}

			ImGui::SliderInt("Audio Filter (Hz)", &viewModel->filterWidth, 0, 12000);

			ImGui::Spacing();

			ImGui::SeparatorText("Notch Filter");

			ImGui::Checkbox("Enable Notch", &viewModel->enableNotch);

			if (viewModel->notchCenterFreq > viewModel->filterWidth) viewModel->notchCenterFreq = viewModel->filterWidth;
			ImGui::SliderInt("Notch Center (Hz)", &viewModel->notchCenterFreq, 0, viewModel->filterWidth);

			ImGui::Spacing();

			ImGui::SeparatorText("Waterfall");

			ImGui::SliderFloat("Min", &viewModel->waterfallMin, -150, 0);

			ImGui::SliderFloat("Max", &viewModel->waterfallMax, -150, 100); ImGui::Spacing();

			ImGui::SeparatorText("Spectre");

			ImGui::SliderFloat("Ratio", &viewModel->ratio, -200, 100);

			ImGui::SliderFloat("Min val", &viewModel->minDb, -200, 0);

			ImGui::SliderInt("Speed", &env->getConfig()->spectre.spectreSpeed, 1, 50);
			ImGui::SliderInt("Speed 2", &env->getConfig()->spectre.spectreSpeed2, 1, 50); ImGui::Spacing();

			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Memory")) {
			ImGui::Spacing();
			memoryRecordUserInterface.drawMemoryBlock(env->getReceiverLogic());
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Device Options")) {
			ImGui::Spacing();
			if (!env->getDeviceController()->isStatusInitOk()) ImGui::BeginDisabled();
			if (ImGui::Button("Stop")) {
				errorInitDeviceShowed = true;
				env->stopProcessing();
			}
			if (!env->getDeviceController()->isStatusInitOk()) ImGui::EndDisabled();
				
			ImGui::SameLine();
			if (env->getDeviceController()->isStatusInitOk()) ImGui::BeginDisabled();
			if (ImGui::Button("Start")) {
				errorInitDeviceShowed = false;
				env->startProcessing();
			}
			if (env->getDeviceController()->isStatusInitOk()) ImGui::EndDisabled();

			ImGui::SameLine();
			if (env->getDeviceController()->isStatusInitOk()) ImGui::BeginDisabled();
			if (ImGui::Button("Reinit")) {
				env->makeReinit();
			}
			if (env->getDeviceController()->isStatusInitOk()) ImGui::EndDisabled();

			selectDeviceLS->drawSetting();

			if (env->getConfig()->deviceType == DeviceType::HACKRF) {
				hackRFsampRateLS->drawSetting();

				ImGui::Spacing();
				ImGui::SeparatorText("RX Settings");

				HackRfInterface* hackRfInterface = (HackRfInterface*)env->getDeviceController()->getDeviceInterface();

				ImGui::SliderInt("LNA Gain", &viewModel->hackRFModel.lnaGain, 0, 5);
				ImGui::SliderInt("VGA Gain", &viewModel->hackRFModel.vgaGain, 0, 31);

				if (hackRfInterface != nullptr) {
					if (hackRfInterface->isDeviceTransmitting()) ImGui::BeginDisabled();
						ImGui::SliderInt("RX AMP", &viewModel->hackRFModel.enableRxAmp, 0, 1);
					if (hackRfInterface->isDeviceTransmitting()) ImGui::EndDisabled();
				}

				hackRFbasebandFilterLS->drawSetting();
			}

			if (env->getConfig()->deviceType == DeviceType::RSP) {
				ImGui::Text("\nRSP Settings:");

				RSPInterface* rspInterface = (RSPInterface*)env->getDeviceController()->getDeviceInterface();

				rspSampRateLS->drawSetting();

				rspDecimationFactorLS->drawSetting();

				ImGui::SliderInt("Gain", &viewModel->rspModel.gain, 20, 59);
				ImGui::Checkbox("Disable LNA", &viewModel->rspModel.lna);

				rspbasebandFilterLS->drawSetting();

				if (rspInterface != nullptr) {
					rspInterface->setGain(viewModel->rspModel.gain, viewModel->rspModel.lna);
					rspInterface->setBasebandFilter(env->getConfig()->rsp.basebandFilter);

					if (env->getDeviceController()->isStatusInitOk()) rspInterface->sendParamsToDevice();
				}
			}

			if (env->getConfig()->deviceType == DeviceType::RTL) {
				rtlSampRateLS->drawSetting();
				rtlDeviceGainLS->drawSetting();

				RTLInterface* rtlInterface = (RTLInterface*)env->getDeviceController()->getDeviceInterface();

				if (rtlInterface != nullptr) {
					rtlInterface->setGain(env->getConfig()->rtl.gain);
					if (env->getDeviceController()->isStatusInitOk()) rtlInterface->sendParamsToDevice();
				}
			}
			ImGui::TreePop();
		}

		if (env->getConfig()->deviceType == DeviceType::HACKRF) {

			HackRfInterface* hackRfInterface = (HackRfInterface*)env->getDeviceController()->getDeviceInterface();

			if (ImGui::TreeNode("TX Settings")) {
				ImGui::Spacing();

				ImGui::Text("Never TX without an antenna connected!"); ImGui::Spacing();

				if (hackRfInterface != nullptr) {

					bool reactionOnSpaceBtn = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space)) && env->getConfig()->transmit.txBySpaceBtn;

					if (hackRfInterface->isDeviceTransmitting() || reactionOnSpaceBtn) ImGui::BeginDisabled();
					if ((ImGui::Button("Start Transmitting") || (reactionOnSpaceBtn && !txSwitcherFlag)) && !hackRfInterface->isDeviceTransmitting()) {
						if (hackRfInterface->pauseRX()) {
							env->getSoundCardInputReader()->continueRead();
							if (hackRfInterface->startTX((int)env->getReceiverLogic()->getFrequencyDelta())) {
								if (reactionOnSpaceBtn) txSwitcherFlag = true;
							}
						}
					}
					ImGui::SameLine(); if (hackRfInterface->isDeviceTransmitting() || reactionOnSpaceBtn) ImGui::EndDisabled();

					if (!hackRfInterface->isDeviceTransmitting() || reactionOnSpaceBtn) ImGui::BeginDisabled();

					if (ImGui::Button("Stop Transmitting") || (!reactionOnSpaceBtn && txSwitcherFlag)) {
						if (hackRfInterface->stopTX()) {
							env->getSoundCardInputReader()->pause();
							if (hackRfInterface->releasePauseRX()) {
								txSwitcherFlag = false;
							}
						}
					};
					if (!hackRfInterface->isDeviceTransmitting() || reactionOnSpaceBtn) ImGui::EndDisabled();
				}

				ImGui::Checkbox("TX by Space btn", &viewModel->transmit.txBySpaceBtn);

				ImGui::SliderInt("TX AMP", &viewModel->hackRFModel.enableTxAmp, 0, 1);
				ImGui::SliderInt("Tx VGA Gain", &viewModel->hackRFModel.txVgaGain, 0, 47);

				ImGui::SliderFloat("Input Level", &viewModel->transmit.inputLevel, 0, 10);
				ImGui::SliderFloat("AM Modulation Depth", &viewModel->transmit.amModulationDepth, 1, 50);
				
				ImGui::Spacing();

				ImGui::TreePop();
			}

			if (hackRfInterface != nullptr) {
				hackRfInterface->setLnaGain((uint32_t)viewModel->hackRFModel.lnaGain);
				hackRfInterface->setVgaGain(viewModel->hackRFModel.vgaGain);
				hackRfInterface->enableRxAmp(viewModel->hackRFModel.enableRxAmp);
				hackRfInterface->enableTxAmp(viewModel->hackRFModel.enableTxAmp);
				hackRfInterface->setTxVgaGain(viewModel->hackRFModel.txVgaGain);
				hackRfInterface->setBaseband(env->getConfig()->hackrf.basebandFilter);

				if (env->getDeviceController()->isStatusInitOk()) hackRfInterface->sendParamsToDevice();
			}
		}

		if (ImGui::TreeNode("Settings")) {
			ImGui::Spacing();
			ImGui::Text("After changing the settings\nmarked with an asterisk\napplication components will be reinitialized.");

			ImGui::Spacing();
			ImGui::SeparatorText("FFT");

			fftLenLS->drawSetting();

			ImGui::Spacing();
			ImGui::SeparatorText("Frequency shift");

			ImGui::InputInt("Shift in Hz", &env->getConfig()->receiver.frequencyShift);
			ImGui::Checkbox("Enable shift", &env->getConfig()->receiver.enableFrequencyShift);

			ImGui::Spacing();
			ImGui::SeparatorText("AGC settings");

			ImGui::InputDouble("Sound threshold", &env->getConfig()->receiver.agc.threshold, 0.001f, 0.1f, "%.3f");
			ImGui::InputDouble("Atack time (ms)", &env->getConfig()->receiver.agc.atackSpeedMs, 0.01f, 0.01f, "%.2f");
			ImGui::InputDouble("Hold time (ms)", &env->getConfig()->receiver.agc.holdingTimeMs, 0.1f, 0.1f, "%.1f");
			ImGui::InputDouble("Release time", &env->getConfig()->receiver.agc.releaseSpeed, 0.00001f, 0.1f, "%.7f");

			ImGui::Spacing();
			ImGui::SeparatorText("Color theme");

			showColorPicker(string("Windows Background"), &env->getConfig()->colorTheme.windowsBGColor, false);
			showColorPicker(string("Main Background"), &env->getConfig()->colorTheme.mainBGColor, false);
			//showColorPicker(string("Window Title Background"), &env->getConfig()->colorTheme.windowsTitleBGColor);
			showColorPicker(string("Spectre Window 1"), &env->getConfig()->colorTheme.spectreWindowFillColor1, false);
			showColorPicker(string("Spectre Window 2"), &env->getConfig()->colorTheme.spectreWindowFillColor2, false);
			showColorPicker(string("Spectre Fill"), &env->getConfig()->colorTheme.spectreFillColor, false);
			ImGui::Checkbox("Spectre gradient", &env->getConfig()->colorTheme.spectreGradientEnable);
			showColorPicker(string("Spectre Profile"), &env->getConfig()->colorTheme.spectreProfileColor, false);
			showColorPicker(string("Receive Region"), &env->getConfig()->colorTheme.receiveRegionColor, true);

			ImGui::Spacing();
			ImGui::SeparatorText("Spectre settings");

			spectreStyleLS->drawSetting();
			ImGui::Checkbox("Contour shows signal power", &env->getConfig()->spectre.contourShowsPower);
			ImGui::SliderFloat("Top coeff", &env->getConfig()->spectre.topCoeff, 0.5f, 1.5f);
			ImGui::SliderFloat("Bottom coeff", &env->getConfig()->spectre.bottomCoeff, 0.5f, 1.5f);
			smoothingDepthLS->drawSetting();
			ImGui::Checkbox("Hang&Decay", &env->getConfig()->spectre.hangAndDecay);
			ImGui::BeginDisabled(!env->getConfig()->spectre.hangAndDecay);
				ImGui::SliderFloat("Decay speed", &env->getConfig()->spectre.decaySpeed, 0, 0.5f, "%.5f");
				ImGui::SliderFloat("Decay speed delta", &env->getConfig()->spectre.decaySpeedDelta, 0, 4);
			ImGui::EndDisabled();

			ImGui::SliderInt("Spectre correction Db", &env->getConfig()->spectre.spectreCorrectionDb, -50, 50);

			ImGui::Spacing();
			ImGui::SeparatorText("Other");

			ImGui::Checkbox("Remove DC", &viewModel->removeDCBias);

			waterfallSpeedLS->drawSetting();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Info")) {
			ImGui::Spacing();
			ImGui::Text("Sampling rate: %d", env->getConfig()->currentWorkingInputSamplerate);
			ImGui::Text("FFT length: %d", env->getConfig()->fftLen);
			viewModel->amp = env->getConfig()->receiver.agc.lastAmp;
			ImGui::Text("AMP: %.2f", viewModel->amp);
			ImGui::Text("CPU usage: %.1f%%", cpu.getCurrentValue());
			ImGui::Text("Buffer available: %.2f sec", viewModel->bufferAvailable);
			ImGui::Text("Service field1: %f", viewModel->serviceField1);
			ImGui::Text("Service field2: %f", viewModel->serviceField2);
			
			ostringstream s;
			vector<int> q = spectre->getDisableControlQueue();
			copy(q.begin(), q.end(), ostream_iterator<int>(s, " "));
			ImGui::Text("Disable cntrl queue: %s", s.str().c_str());
		
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("About")) {
			ImGui::Spacing();
			string msg;
			msg.append(APP_NAME).append("\n\n");
			msg.append("For all questions related to our software,\nplease, contact our email box: dkcocto@gmail.com.\n\n");
			msg.append("We will be glad to hear your wishes and suggestions.");

			ImGui::Text(msg.c_str());

			ImGui::TreePop();
		}
		
		//Если вкладка опций устройства не выбрана, то все равно устанавливаем конфигурацию на текущее устройство
		if (env->getDeviceController()->isStatusInitOk()) {
			DeviceInterface* deviceInterface = env->getDeviceController()->getDeviceInterface();
			if (deviceInterface != nullptr) {
				if (env->getDeviceController()->getCurrentDeviceType() == HACKRF) {
					((HackRfInterface*)deviceInterface)->setFreq(viewModel->centerFrequency);
					((HackRfInterface*)deviceInterface)->sendParamsToDevice();
				}
				if (env->getDeviceController()->getCurrentDeviceType() == RTL) {
					((RTLInterface*)deviceInterface)->setFreq(viewModel->centerFrequency);
					((RTLInterface*)deviceInterface)->sendParamsToDevice();
				}
				if (env->getDeviceController()->getCurrentDeviceType() == RSP) {
					((RSPInterface*)deviceInterface)->setFreq(viewModel->centerFrequency);
					((RSPInterface*)deviceInterface)->sendParamsToDevice();
				}
			}
		}

		//Danger нужно проверить этот момент
		viewModel->storeToConfig();

	ImGui::End();
	//ImGui::PopID();

	spectre->draw();

	if (env->getDeviceController()->isStatusInitFail()) {
		showAlertOKDialog(std::string("Warning"), std::string("Application couldn't init a selected device.\nPlease, go to settings and select the correct device or plug your device to USB port.\nMake sure you have selected the correct api version in the settings for RSP devices.\n\nReturned answer:\n\n").append(env->getDeviceController()->getResult()->err));
		if (!errorInitDeviceShowed) {
			spectre->disableControlForID(DISABLE_CONTROL_DIALOG);
			ImGui::OpenPopup(std::string("Warning").c_str());
			errorInitDeviceShowed = true;
		}
	}

	ImGui::PopStyleColor();

	env->getConfig()->lastSelectedFreq = env->getReceiverLogic()->getSelectedFreqNew(); //saving last selected freq to env->getConfig() class
	
	auto range = env->getFlowingSpectre()->getVisibleFreqsRangeAbsolute();
	env->getConfig()->spectre.visibleStartFreq = range.first;
	env->getConfig()->spectre.visibleStopFreq = range.second;
}

void Display::showSelectDeviceSetting() {
	const char* items[] = { "RSP1/RSP1A", "HackRF", "RTLSDR" };
	static int item_current_idx = env->getConfig()->deviceType; // Here we store our selection data as an index.
	const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("Device (*)", combo_preview_value, 0)) {
		for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				item_current_idx = n;
				env->getConfig()->setDevice(item_current_idx);
			}
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void Display::showHackrfSamplingRateSetting() {
	const char* items[] = { "2000000", "4000000", "5000000", "8000000", "10000000", "12500000", "16000000", "20000000" };
	static int item_current_idx = env->getConfig()->deviceType; // Here we store our selection data as an index.
	const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("Sampling rate (*)", combo_preview_value, 0)) {
		for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				item_current_idx = n;
				uint32_t samplingRate;
				Utils::parse_u32((char*)items[n], &samplingRate);
				env->getConfig()->hackrf.deviceSamplingRate = samplingRate;
			}
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void Display::showAlertOKDialog(std::string title, std::string msg) {
	if (ImGui::BeginPopupModal(title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(msg.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { spectre->enableControlForID(DISABLE_CONTROL_DIALOG); ImGui::CloseCurrentPopup(); }
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

ViewModel* Display::getViewModel() {
	return viewModel;
}

void Display::showColorPicker(string title, unsigned int* configVal, bool withTransparency) {
	static bool hdr = false;
	static bool drag_and_drop = false;
	static bool alpha_half_preview = false;
	static bool options_menu = false;
	static bool alpha_preview = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	//static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 1.0f);

	ImVec4 color = ImGui::ColorConvertU32ToFloat4(*configVal);

	bool isColorPicked = false;

	if (!withTransparency) {
		isColorPicked = ImGui::ColorEdit3(title.c_str(), (float*)&color, misc_flags);
	} else {
		isColorPicked = ImGui::ColorEdit4(title.c_str(), (float*)&color, misc_flags);
	}

	if (isColorPicked) {
		spectre->disableControlForID(DISABLE_CONTROL_COLOR_PICKER);
	}
	else {
		spectre->enableControlForID(DISABLE_CONTROL_COLOR_PICKER);
	}

	*configVal = ImGui::ColorConvertFloat4ToU32(color);
}


void Display::initDynamicSettings() {
	/*std::map<int, std::string> decimationMap;
	for (int i = 1, j = 0; i <= 64; i++) {
		if (env->getConfig()->deviceType == DeviceType::HACKRF) {
			if (env->getConfig()->hackrf.deviceSamplingRate % i == 0) {
				decimationMap.insert(pair<int, std::string>{j, to_string(i)});
				j++;
			}

		}
		if (env->getConfig()->deviceType == DeviceType::RSP) {
			if ((env->getConfig()->rsp.deviceSamplingRate / env->getConfig()->rsp.deviceDecimationFactor) % i == 0) {
				decimationMap.insert(pair<int, std::string>{j, to_string(i)});
				j++;
			}
		}
		if (env->getConfig()->deviceType == DeviceType::RTL) {
			if (env->getConfig()->rtl.deviceSamplingRate % i == 0) {
				decimationMap.insert(pair<int, std::string>{j, to_string(i)});
				j++;
			}
		}
	}
	decimationLS = std::make_unique<ListSetting>(env, decimationMap, "Decimation", true);
	decimationLS->bindVariable(&env->getConfig()->delayedInputSamplerateDivider);*/

	if (env->getConfig()->deviceType == DeviceType::RSP) {
		std::map<int, std::string> rspDecimationFactorMap;
		for (int i = 1, j = 0; i <= 32; i++) {
			if (env->getConfig()->rsp.deviceSamplingRate % i == 0 && (i & (i - 1)) == 0) {
				rspDecimationFactorMap.insert(pair<int, std::string>{i, to_string(i)});
				j++;
			}
		}

		rspDecimationFactorLS = std::make_unique<ListSetting<int>>(env, rspDecimationFactorMap, "Decimation factor", true);
		rspDecimationFactorLS->bindVariable(&env->getConfig()->rsp.deviceDecimationFactor);
	}
}

void Display::initSettings() {
	//auto begin = std::chrono::steady_clock::now();

	std::map<DeviceType, std::string> selectDeviceMap = {
		{RSP , "RSP"},
		{HACKRF , "HACKRF"},
		{RTL , "RTL"}
	};

	selectDeviceLS = std::make_unique<ListSetting<DeviceType>>(env, selectDeviceMap, "Select device", true);
	selectDeviceLS->bindVariable(&env->getConfig()->delayedDeviceType);

	std::map<int, std::string> hackRFsamplingRateMap = {
		{2000000 , "2000000"},
		{4000000 , "4000000"},
		{5000000 , "5000000"},
		{8000000 , "8000000"},
		{10000000 , "10000000"},
		{12500000 , "12500000"},
		{16000000 , "16000000"},
		{20000000 , "20000000"}
	};

	hackRFsampRateLS = std::make_unique<ListSetting<int>>(env, hackRFsamplingRateMap, "Sampling rate", true);
	hackRFsampRateLS->bindVariable(&env->getConfig()->hackrf.deviceSamplingRate);

	//const char* items[] = { "1750000", "2500000", "3500000", "5000000", "5500000", "6000000", "7000000", "8000000", "9000000", "10000000", "20000000" };
	std::map<int, std::string> hackRFBasebandFilterMap = {
		{1750000 , "1750000"},
		{2500000 , "2500000"},
		{3500000 , "3500000"},
		{5000000 , "5000000"},
		{5500000 , "5500000"},
		{6000000 , "6000000"},
		{7000000 , "7000000"},
		{8000000 , "8000000"},
		{9000000 , "9000000"},
		{10000000 , "10000000"},
		{20000000 , "20000000"}
	};

	hackRFbasebandFilterLS = std::make_unique<ListSetting<int>>(env, hackRFBasebandFilterMap, "Baseband filter", false);
	hackRFbasebandFilterLS->bindVariable(&env->getConfig()->hackrf.basebandFilter);
	//--------------------


	//-------RSP settings-
	std::map<int, std::string> rspSamplingRateMap = {
		{2000000 , "2000000"},
		{3000000 , "3000000"},
		{4000000 , "4000000"},
		{6000000 , "6000000"},
		{7000000 , "7000000"},
		{8000000 , "8000000"},
		{10000000 , "10000000"}
	};

	rspSampRateLS = std::make_unique<ListSetting<int>>(env, rspSamplingRateMap, "Sampling rate", true);
	rspSampRateLS->bindVariable(&env->getConfig()->rsp.deviceSamplingRate);


	std::map<int, std::string> rspbasebandFilterMap = {
		{0 , "0"},
		{200 , "200"},
		{300 , "300"},
		{600 , "600"},
		{1536 , "1536"},
		{5000 , "5000"},
		{6000 , "6000"},
		{7000 , "7000"},
		{8000 , "8000"}
	};
	rspbasebandFilterLS = std::make_unique<ListSetting<int>>(env, rspbasebandFilterMap, "Baseband Filter", false);
	rspbasebandFilterLS->bindVariable(&env->getConfig()->rsp.basebandFilter);
	//--------------------



	//-10, 15, 40, 65, 90, 115, 140, 165, 190,
	//215, 240, 290, 340, 420, 430, 450, 470, 490
	std::map<int, std::string> rtlGainMap = {
		{-10 , "-10"},
		{15 , "15"},
		{40 , "40"},
		{65 , "65"},
		{90 , "90"},
		{115 , "115"},
		{140 , "140"},
		{165 , "165"},
		{190 , "190"},
		{215 , "215"},
		{240 , "240"},
		{290 , "290"},
		{340 , "340"},
		{420 , "420"},
		{430 , "430"},
		{450 , "450"},
		{470 , "470"},
		{490 , "490"}
	};
	rtlDeviceGainLS = std::make_unique<ListSetting<int>>(env, rtlGainMap, "Gain", false);
	rtlDeviceGainLS->bindVariable(&env->getConfig()->rtl.gain);

	//225001 - 300000 Hz
	//900001 - 3200000 Hz
	std::map<int, std::string> rtlSampRateMap = {
		{250000 , "250000"},
		{300000 , "300000"},
		{1000000 , "1000000"},
		{1500000 , "1500000"},
		{2000000 , "2000000"},
		{2500000 , "2500000"},
		{3000000 , "3000000"},
		{3200000 , "3200000"}
	};

	rtlSampRateLS = std::make_unique<ListSetting<int>>(env, rtlSampRateMap, "Sampling rate", true);
	rtlSampRateLS->bindVariable(&env->getConfig()->rtl.deviceSamplingRate);


	//-------------Spectre
	std::map<int, std::string> spectreStyleMap = { {0, "Colored contour"}, {1, "Only contour"},	{2, "Color filled"} };

	spectreStyleLS = std::make_unique<ListSetting<int>>(env, spectreStyleMap, "Spectre style", false);
	spectreStyleLS->bindVariable(&env->getConfig()->spectre.style);

	std::map<int, std::string> smoothingDepthMap = {
		{0, "0"},
		{1, "1"},
		{2, "2"},
		{3, "3"},
		{4, "4"},
		{5, "5"},
		{6, "6"},
		{7, "7"},
		{8, "8"}
	};
	smoothingDepthLS = std::make_unique<ListSetting<int>>(env, smoothingDepthMap, "Smoothing depth", false);
	smoothingDepthLS->bindVariable(&env->getConfig()->spectre.smoothingDepth);
	//--------------------

	//--------------Other-
	std::map<int, std::string> fftLenMap = {
		{ 8192, "8192"},
		{ 16384, "16384" },
		{ 32768, "32768" },
		{ 65536, "65536" },
		{ 131072, "131072" },
		{ 262144, "262144" },
		{ 524288, "524288" }
	};
	fftLenLS = std::make_unique<ListSetting<int>>(env, fftLenMap, "FFT length", true);
	fftLenLS->bindVariable(&env->getConfig()->delayedFFTLen);

	std::map<int, std::string> waterfallSpeedMap = {
		{1, "1"},
		{2, "2"},
		{3, "3"}
	};
	waterfallSpeedLS = std::make_unique<ListSetting<int>>(env, waterfallSpeedMap, "Waterfall speed", false);
	waterfallSpeedLS->bindVariable(&env->getConfig()->waterfall.speed);
	//--------------------

	/*auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	std::cout << "The time: " << elapsed_ms.count() << " micros\n";*/
}