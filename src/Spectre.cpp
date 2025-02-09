#include "Spectre.h"

#define BLUE						IM_COL32(88, 88, 231, 255)

#define BASE_COLOR					IM_COL32(10, 10, 10, 100)

#define VIOLET						IM_COL32(72, 3, 111, 160)
#define RED							IM_COL32(166, 0, 0, 80)

#define YELLOW						IM_COL32(255, 255, 0, 255)
#define SPECTRE_COLOR				IM_COL32(50, 50, 50, 255)
#define SPECTRE_CONTUR_COLOR		IM_COL32(255, 255, 255, 255)

#define SPECTRE_FREQ_MARK_COUNT		20
#define SPECTRE_FREQ_MARK_COUNT_DIV	10
#define SPECTRE_DB_MARK_COUNT		10

//Upper right (spectreX1, spectreY1), down left (spectreX2, spectreY2)
bool Spectre::isMouseOnRegion(Spectre::Region region) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.MousePos.x >= region.x1.x && io.MousePos.x <= region.x2.x && io.MousePos.y >= region.x1.y && io.MousePos.y <= region.x2.y) return true;
	return false;
}

void Spectre::waterfallAutoColorCorrection() {
	viewModel->waterfallMin = minMax.min;
	viewModel->waterfallMax = minMax.average + (minMax.max - minMax.min) + 3;
}

void Spectre::spectreRatioAutoCorrection() {
	viewModel->minDb = minMax.average - 8;
	viewModel->ratio = minMax.max + 30;
	//config->spectre.spectreCorrectionDb = viewModel->minDb;
}

vector<int> Spectre::getDisableControlQueue() {
	return disableControlQueue;
}

Spectre::Spectre(Environment* env) {
	this->env = env;

	this->config = env->getConfig();
	this->viewModel = env->getViewModel();

	ratioKalman = make_unique<KalmanFilter>(1, 0.01);
	spectreTranferKalman = make_unique<KalmanFilter>(1, 0.01);

	this->waterfall = make_unique<Waterfall>(config, viewModel);

	receiverRegionInterface = make_unique<ReceiverRegionInterface>(&sWD, config, viewModel, env);
}

int savedStartWindowX = 0;
int savedEndWindowX = 0;
int savedSpectreWidthInPX = 1;

float veryMinSpectreVal = -100;
float veryMaxSpectreVal = -60;

int countFrames = 0;

bool isFirstFrame = true;

bool controlButtonHovered = false;

void Spectre::draw() {

	env->getReceiverLogic()->setCenterFrequency(viewModel->centerFrequency);

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Spectre");
		//Ќачальна¤ точка окна	
		sWD.startWindowPoint = ImGui::GetCursorScreenPos();

		//Ќижн¤¤ лева¤ точка окна
		sWD.windowLeftBottomCorner = ImGui::GetContentRegionAvail();

		windowFrame.UPPER_RIGHT = sWD.startWindowPoint;
		windowFrame.BOTTOM_LEFT = ImVec2(sWD.startWindowPoint.x + sWD.windowLeftBottomCorner.x, sWD.startWindowPoint.y + sWD.windowLeftBottomCorner.y);

		spectreHeight = sWD.windowLeftBottomCorner.y / 2.0 * spectreWaterfallRelations;
		spectreWidth = sWD.windowLeftBottomCorner.x - sWD.rightPadding - sWD.leftPadding;

		handleEvents(spectreWidth, env->getReceiverLogic(), env->getFlowingSpectre());

		FFTData::OUTPUT* fullSpectreData = env->getFFTSpectreHandler()->getFFTData()->getDataCopy(false);
		FFTData::OUTPUT* fullWaterfallData = env->getFFTSpectreHandler()->getFFTData()->getDataCopy(true);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::BeginChild("Spectre1", ImVec2(ImGui::GetContentRegionAvail().x, spectreHeight), false, ImGuiWindowFlags_NoMove);

			ImGui::SetCursorPos(ImVec2(spectreWidth - 90, 10));

			if (ImGui::Button("Auto")) {
				spectreRatioAutoCorrection();
				waterfallAutoColorCorrection();
			}
			bool spectreAutoButtonHovered = ImGui::IsItemHovered();
			ImGui::SameLine();

			if (ImGui::Button("<|>")) { env->getReceiverLogic()->setReceivedFreqToSpectreCenter(); waterfall->clear(); }
			bool freqToCenterButtonHovered = ImGui::IsItemHovered();
			ImGui::SameLine();

			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(
					"Help:\nCtrl + Mouse wheel: zoomIn/zoomOut\nMouse wheel: +- 100 hz\nLMB + drag: tunning\nLMB on the spectre + drag: set frequency/tunning\nRMB + drag: moving the spectre\n\nLMB: left mouse button\nRMB: right mouse button");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			if (spectreAutoButtonHovered || freqToCenterButtonHovered) disableControlForID(DISABLE_CONTROL_SPECTRE_BUTTONS); else {
				enableControlForID(DISABLE_CONTROL_SPECTRE_BUTTONS);
			}

			ImGui::SetCursorPos(ImVec2(0, 0));

			drawMemoryMarks(draw_list, env->getFlowingSpectre(), env->getReceiverLogic());

			drawSpectreContour(fullSpectreData, draw_list, env->getFlowingSpectre(), env->getFFTSpectreHandler());

			//dB mark line
			float stepInPX = (float)spectreHeight / (float)SPECTRE_DB_MARK_COUNT;
			float stepdB = (abs(veryMinSpectreVal - viewModel->ratio)) / SPECTRE_DB_MARK_COUNT;
			config->spectre.minVisibleDB = veryMinSpectreVal;
			config->spectre.maxVisibleDB = (int)round(veryMinSpectreVal + (SPECTRE_DB_MARK_COUNT - 1) * stepdB);


			for (int i = 0; i < SPECTRE_DB_MARK_COUNT; i++) {
				int dbMark = (int)round(veryMinSpectreVal + i * stepdB);
				if (dbMark <= 0) {
					draw_list->AddText(
						ImVec2(sWD.startWindowPoint.x + sWD.rightPadding - 40, sWD.startWindowPoint.y + spectreHeight - i * stepInPX - 15),
						IM_COL32_WHITE,
						std::to_string(dbMark).c_str()
					);
				}
			}
			//---------------
			
			drawFreqMarks(draw_list, sWD.startWindowPoint, sWD.windowLeftBottomCorner, spectreWidth, spectreHeight, env->getFlowingSpectre());

			//TX/RX MARK
			/*bool tx = false;
			if (env->getDeviceController()->getDevice() != nullptr && env->getDeviceController()->getDevice()->deviceType == HACKRF) {
				tx = ((HackRFDevice*)env->getDeviceController()->getDevice())->isDeviceTransmitting();
			}
			ImGui::PushFont(viewModel->fontBigRegular);
			draw_list->AddText(
				ImVec2(sWD.startWindowPoint[0] + 550, 150),
				(tx == true) ? LIGHTRED : GREEN,
				(tx == true) ? "TX" : "RX"
			);
			ImGui::PopFont();*/
			//----------

		ImGui::EndChild();

		ImGui::BeginChild("Waterfall", ImVec2(ImGui::GetContentRegionAvail().x, sWD.windowLeftBottomCorner.y - spectreHeight - 5), false, ImGuiWindowFlags_NoMove);
			
			if (env->reloading) waterfall->clear();

			if (env->getDeviceController()->isStatusInitOk()) {
				if (countFrames % config->waterfall.speed == 0) {
					waterfall->setMinMaxValue(viewModel->waterfallMin, viewModel->waterfallMax);
					waterfall->update(fullWaterfallData, env->getFlowingSpectre(), env->getFFTSpectreHandler());
				}
			}
			 
			int waterfallHeight = sWD.windowLeftBottomCorner.y - spectreHeight - sWD.waterfallPaddingTop;

			int spectreSize = env->getFlowingSpectre()->getLen();

			float stepX = spectreWidth / (spectreSize / waterfall->getDiv());
			float stepY = (float)(waterfallHeight + sWD.waterfallPaddingTop) / (float)waterfall->getSize();

			for (int i = 0; i < waterfall->getSize(); i++) {
				draw_list->AddImage(
					(void*)(intptr_t)waterfall->getTexturesArray()[i],
					ImVec2(
						sWD.startWindowPoint.x + sWD.rightPadding,
						sWD.startWindowPoint.y + spectreHeight + sWD.waterfallPaddingTop + stepY * i
					),
					ImVec2(
						sWD.startWindowPoint.x + sWD.windowLeftBottomCorner.x - sWD.leftPadding,
						sWD.startWindowPoint.y + spectreHeight + sWD.waterfallPaddingTop + stepY * (i + 1)
					));
			}

			//receive region
			receiverRegionInterface->drawRegion(draw_list, env->getReceiverLogic(), fullWaterfallData);
			//--
		ImGui::EndChild();

		if (!isControlDisabled() && !receiverRegionInterface->isDigitSelected()) drawFreqPointerMark(sWD.startWindowPoint, sWD.windowLeftBottomCorner, spectreWidth, draw_list, env->getReceiverLogic());

	ImGui::End();

	if (isFirstFrame) {
		env->getReceiverLogic()->setFreq(config->lastSelectedFreq); //load last selected freq
		isFirstFrame = false;
	}
	countFrames++;
	env->getFFTSpectreHandler()->getFFTData()->destroyData(fullSpectreData);
	env->getFFTSpectreHandler()->getFFTData()->destroyData(fullWaterfallData);
}

Spectre::MIN_MAX Spectre::getMinMaxInSpectre() {
	return minMax;
}

bool Spectre::isControlDisabled() {
	return disableControlQueue.size() != 0;
}

void Spectre::releaseControl() {
	disableControlQueue.clear();
}

void Spectre::disableControlForID(int id) {
	if (std::find(disableControlQueue.begin(), disableControlQueue.end(), id) == disableControlQueue.end()) {
		disableControlQueue.push_back(id);
	}
}

void Spectre::enableControlForID(int id) {
	auto foundID = std::find(disableControlQueue.begin(), disableControlQueue.end(), id);
	if (foundID != disableControlQueue.end()) {
		int index = foundID - disableControlQueue.begin();
		disableControlQueue.erase(disableControlQueue.begin() + index);
	}
}

Spectre::MIN_MAX Spectre::getMinMaxInSpectre(std::vector<float> spectreData, int len) {
	float min = 1000.0;
	float max = -1000.0;
	float sum = 0;
	for (int i = 0; i < len; i++) {
		if (spectreData[i] < min) min = spectreData[i];
		if (spectreData[i] > max) max = spectreData[i];
		sum += spectreData[i];
	}
	return MIN_MAX { min, max, sum / len };
}

void Spectre::handleEvents(int spectreWidthInPX, ReceiverLogic* receiverLogic, FlowingSpectre* flowingSpec) {
	if (isControlDisabled()) return;

	bool isMouseOnSpectre = isMouseOnRegion(
		Region {
			ImVec2 (sWD.startWindowPoint.x + sWD.rightPadding, sWD.startWindowPoint.y),
			ImVec2 (sWD.startWindowPoint.x + sWD.windowLeftBottomCorner.x - sWD.leftPadding, sWD.startWindowPoint.y + spectreHeight)
		});

	bool isMouseOnWaterfall = isMouseOnRegion(
		Region{
			ImVec2(sWD.startWindowPoint.x + sWD.rightPadding, sWD.startWindowPoint.y + spectreHeight),
			ImVec2(sWD.startWindowPoint.x + sWD.windowLeftBottomCorner.x - sWD.leftPadding, sWD.startWindowPoint.y + 2 * spectreHeight)
		});

	bool isMouseOnReceiverFreq = receiverRegionInterface->isDigitSelected();

	ImGuiIO& io = ImGui::GetIO();

	if (!viewModel->mouseBusy && (isMouseOnSpectre || isMouseOnWaterfall)) {

		bool ctrlPressed = false;

		struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } };
		for (ImGuiKey key = (ImGuiKey)0; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) {
			if (funcs::IsLegacyNativeDupe(key)) continue;
			if (ImGui::IsKeyDown(key)) {
				if (key == 527) {
					ctrlPressed = true;
				}
			}
		}

		float mouseWheelVal = io.MouseWheel;
		if (mouseWheelVal != 0) {
			if (isMouseOnReceiverFreq) {
				receiverRegionInterface->setupNewFreq(mouseWheelVal > 0, receiverLogic);
			} else {
				if (!ctrlPressed) {
					int mouseWheelStep = 100;
					int selectedFreq = receiverLogic->getSelectedFreqNew();
					int selectedFreqShortByStep = selectedFreq / mouseWheelStep;
					if (mouseWheelVal > 0) receiverLogic->setFreq(mouseWheelStep * selectedFreqShortByStep + mouseWheelVal * mouseWheelStep);
					else {
						if (selectedFreq - selectedFreqShortByStep * mouseWheelStep > 0) {
							receiverLogic->setFreq(mouseWheelStep * selectedFreqShortByStep);
						} else {
							receiverLogic->setFreq(mouseWheelStep * selectedFreqShortByStep + mouseWheelVal * mouseWheelStep);
						}
					}
				}
				else {
					if (mouseWheelVal > 0) {
						flowingSpec->zoomIn();
					} else {
						flowingSpec->zoomOut();
					}
					receiverLogic->setFrequencyDelta(receiverLogic->getFrequencyDelta());
					waterfall->clear();
					spectreRatioAutoCorrection();
					waterfallAutoColorCorrection();
				}
			}
		}
	}

	//¬ыполн¤етс¤ если Ќажатие мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
	if (!viewModel->mouseBusy && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && (isMouseOnSpectre || isMouseOnWaterfall)) {
		if (ImGui::IsMouseClicked(0) && isMouseOnSpectre && !isMouseOnWaterfall) {
			receiverLogic->saveSpectrePositionDelta(getMousePosXOnSpectreWindow());
		}
		if (ImGui::IsMouseClicked(1) && (isMouseOnWaterfall || isMouseOnSpectre)) {
			flowingSpec->prepareForMovingSpectreByMouse(getMousePosXOnSpectreWindow());
		}
	}

	if (!viewModel->mouseBusy && ImGui::IsMouseDown(0) && isMouseOnWaterfall) {
		if (ImGui::IsMouseDown(0)) {
			int cursorPos = getMousePosXOnSpectreWindow();
			float freq = receiverLogic->getFreqByPosOnSpectrePx(cursorPos);
			receiverLogic->setFreq(freq);
		}
	}

	//¬ыполн¤етс¤ если удержание кнопки мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
	if (!viewModel->mouseBusy && (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1)) && (isMouseOnSpectre || isMouseOnWaterfall)) {
		if (ImGui::IsMouseDown(0) && isMouseOnSpectre && !isMouseOnWaterfall) {
			receiverLogic->setFrequencyDeltaFromSavedPosition((float)getMousePosXOnSpectreWindow());
		}
		if (ImGui::IsMouseDown(1) && (isMouseOnWaterfall || isMouseOnSpectre)) {
			float newCenterFreq = flowingSpec->moveSpectreByMouse(spectreWidthInPX, getMousePosXOnSpectreWindow());

			receiverLogic->setFrequencyDelta(receiverLogic->getFrequencyDelta());
			//waterfall->clear();
		}
	}

	//ќпредел¤ем сдвинулось ли окно по x
	if (savedStartWindowX != sWD.startWindowPoint.x) {
		savedStartWindowX = sWD.startWindowPoint.x;
	}

	//ќпредел¤ем изменилс¤ ли размер окна по x
	if (savedEndWindowX != sWD.windowLeftBottomCorner.x) {
		receiverLogic->updateSpectreWidth(savedSpectreWidthInPX, spectreWidthInPX);
		savedSpectreWidthInPX = spectreWidthInPX;
		savedEndWindowX = sWD.windowLeftBottomCorner.x;
	}
}

int Spectre::getMousePosXOnSpectreWindow() {
	ImGuiIO& io = ImGui::GetIO();
	return io.MousePos.x - (sWD.startWindowPoint.x + sWD.rightPadding);
}

void Spectre::drawFreqMarks(ImDrawList* draw_list, ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, int spectreHeight, FlowingSpectre* flowingSpec) {
	//Horizontal
	draw_list->AddLine(
		ImVec2(startWindowPoint.x + sWD.rightPadding, startWindowPoint.y + spectreHeight),
		ImVec2(startWindowPoint.x + windowLeftBottomCorner.x - sWD.leftPadding, startWindowPoint.y + spectreHeight),
		GRAY, 4.0f);

	//Vertical
	draw_list->AddLine(
		ImVec2(startWindowPoint.x + sWD.rightPadding, 0),
		ImVec2(startWindowPoint.x + sWD.rightPadding, startWindowPoint.y + spectreHeight),
		GRAY, 2.0f);
	
	//Freqs mark line
	//Отметки частоты должны зависить от ширины окна спектра, подобрал такую зависимость
	int markCount = (int)(0.008 * windowLeftBottomCorner.x) * SPECTRE_FREQ_MARK_COUNT_DIV;
	
	FlowingSpectre::FREQ_RANGE freqRange = flowingSpec->getVisibleFreqRangeFromSamplerate();

	float freqStep = (freqRange.second - freqRange.first) / (float)markCount;
	float stepInPX = (float)spectreWidthInPX / (float)markCount;

	for (int i = 0; i <= markCount; i++) {
		if (i % SPECTRE_FREQ_MARK_COUNT_DIV == 0) {
			//std::to_string().c_str()
			draw_list->AddText(
				ImVec2(startWindowPoint.x + sWD.rightPadding + i * stepInPX - 25.0, startWindowPoint.y + spectreHeight + 10.0),
				IM_COL32_WHITE,
				Utils::getShortPrittyFreq((int)(flowingSpec->getVisibleStartFrequency() + (float)i * freqStep)).c_str()
			);
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + sWD.rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
				ImVec2(startWindowPoint.x + sWD.rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 9.0),
				IM_COL32_WHITE, 2.0f
			);
		}
		else {
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + sWD.rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
				ImVec2(startWindowPoint.x + sWD.rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 5.0),
				IM_COL32_WHITE, 2.0f
			);
		}
	}
	//-----------------------------
}

int MAX_THICKNESS = 2.0f;

void Spectre::drawSpectreContour(FFTData::OUTPUT* fullSpectreData, ImDrawList* draw_list, FlowingSpectre* flowingSpec, SpectreHandler* specHandler) {

	std::vector<float> reducedSpectreData = flowingSpec->getReducedData(fullSpectreData, config->visibleSpectreBinCount, specHandler);

	minMax = getMinMaxInSpectre(reducedSpectreData, reducedSpectreData.size());
	config->spectre.minDb = minMax.min;
	config->spectre.maxDb = minMax.max;
	config->spectre.averageDb = minMax.average;

	//printf("%f\r\n", minMax.average);

	if (config->spectre.style == 2) {
		coloredSpectreBG.generateImage(waterfall.get(), ColoredSpectreBG::params{100, spectreHeight, viewModel->waterfallMin * config->spectre.bottomCoeff, viewModel->waterfallMax * config->spectre.topCoeff, viewModel->waterfallMin, viewModel->waterfallMax});
		draw_list->AddImage(
			(void*)(intptr_t)coloredSpectreBG.getImage(),
			ImVec2(
				sWD.startWindowPoint.x,
				sWD.startWindowPoint.y - 8
			),
			ImVec2(
				sWD.startWindowPoint.x + sWD.rightPadding + spectreWidth,
				sWD.startWindowPoint.y + spectreHeight
			)
		);
	}


	veryMinSpectreVal = viewModel->minDb;
	if (veryMaxSpectreVal < minMax.max) veryMaxSpectreVal = minMax.max;

	double stepX = (double)(sWD.windowLeftBottomCorner.x - sWD.rightPadding - sWD.leftPadding) / (double)(reducedSpectreData.size() - 1.0);

	//double ratio = ratioKalman->filter((double)spectreHeight / (abs(veryMinSpectreVal - viewModel->ratio)));
	double ratio = (double)spectreHeight / (abs(veryMinSpectreVal - viewModel->ratio));

	double koeff = 0;
	if (abs(veryMinSpectreVal) * ratio > (double)spectreHeight) {
		koeff = (-1.0) * (abs(veryMinSpectreVal) * ratio - (double)spectreHeight);
	}
	else {
		koeff = (double)spectreHeight - abs(veryMinSpectreVal) * ratio;
	}

	//koeff = spectreTranferKalman->filter(koeff);
	//koeff = koeff;


	if (config->spectre.style == 0 || config->spectre.style == 1) {
		//Waterfall::RGB minRGB = waterfall->getColorForPowerInSpectre(viewModel->waterfallMin);
		//Waterfall::RGB maxRGB = waterfall->getColorForPowerInSpectre(-30);	

		if (config->colorTheme.spectreGradientEnable) {
			draw_list->AddRectFilledMultiColor(
				ImVec2(
					sWD.startWindowPoint.x + sWD.rightPadding,
					sWD.startWindowPoint.y - 8
				),
				ImVec2(
					sWD.startWindowPoint.x + spectreWidth + sWD.leftPadding,
					sWD.startWindowPoint.y + spectreHeight
				),
				config->colorTheme.spectreWindowFillColor1,
				config->colorTheme.spectreWindowFillColor1,
				config->colorTheme.spectreWindowFillColor2,
				config->colorTheme.spectreWindowFillColor2
			);
		} else {
			draw_list->AddRectFilled(
				ImVec2(
					sWD.startWindowPoint.x + sWD.rightPadding,
					sWD.startWindowPoint.y - 8
				),
				ImVec2(
					sWD.startWindowPoint.x + spectreWidth + sWD.leftPadding,
					sWD.startWindowPoint.y + spectreHeight
				),
				config->colorTheme.spectreWindowFillColor1
			);
		}
	}

	//auto begin = std::chrono::steady_clock::now();

	for (int i = 0; i < reducedSpectreData.size() - 1; i++) {

		float y1 = round(sWD.startWindowPoint.y - reducedSpectreData[i] * ratio + koeff);
		if (y1 >= sWD.startWindowPoint.y + spectreHeight) y1 = sWD.startWindowPoint.y + spectreHeight;

		float y2 = round(sWD.startWindowPoint.y - reducedSpectreData[i + 1] * ratio + koeff);
		if (y2 >= sWD.startWindowPoint.y + spectreHeight) y2 = sWD.startWindowPoint.y + spectreHeight;

		double thickness = (double)(((config->visibleSpectreBinCount - reducedSpectreData.size() + 1)) - 0) / (double)(config->visibleSpectreBinCount - 0) * (MAX_THICKNESS - 1.0f) + 1.0f;

		if (config->spectre.style == 0) {

			ImVec2 lineX1(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				y1
			);
			ImVec2 lineX2(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				y2
			);

			ImVec2 lineX3(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				sWD.startWindowPoint.y + spectreHeight
			);

			ImVec2 lineX4(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				sWD.startWindowPoint.y + spectreHeight
			);

			ImVec2* polygon = new ImVec2[]{ lineX1 , lineX2 , lineX3 , lineX4 };
			draw_list->AddConvexPolyFilled(polygon, 4, config->colorTheme.spectreFillColor);
			delete[] polygon;

			lineX1 = ImVec2 (
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				y1 - thickness
			);
			lineX2 = ImVec2 (
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				y2 - thickness
			);

			if (config->spectre.contourShowsPower) {
				Waterfall::RGB powerRGB = waterfall->getColorForPowerInSpectre(reducedSpectreData[i], viewModel->waterfallMin * config->spectre.topCoeff, viewModel->waterfallMax * config->spectre.bottomCoeff);
				draw_list->AddLine(lineX1, lineX2, IM_COL32(powerRGB.r, powerRGB.g, powerRGB.b, 255), thickness);
			} else {
				draw_list->AddLine(lineX1, lineX2, config->colorTheme.spectreProfileColor, thickness);
			}

		}

		if (config->spectre.style == 2) {
			ImVec2 lineX1(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				y1
			);
			ImVec2 lineX2(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				y2
			);

			ImVec2 lineX3(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				sWD.startWindowPoint.y - 8
			);

			ImVec2 lineX4(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				sWD.startWindowPoint.y - 8
			);

			ImVec2* polygon = new ImVec2[]{ lineX3 , lineX4 , lineX1 , lineX2 };
			draw_list->AddConvexPolyFilled(polygon, 4, config->colorTheme.spectreWindowFillColor1);
			delete[] polygon;

			ImVec2 profiledX1(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				y1
			);
			ImVec2 profiledX2(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				y2
			);

			draw_list->AddLine(profiledX1, profiledX2, config->colorTheme.spectreProfileColor, thickness);
		}

		if (config->spectre.style == 1) {
			ImVec2 lineX1(
				sWD.startWindowPoint.x + round(i * stepX) + sWD.rightPadding,
				y1
			);
			ImVec2 lineX2(
				sWD.startWindowPoint.x + round((i + 1) * stepX) + sWD.rightPadding,
				y2
			);

			//float thickness = (((float)config->visibleSpectreBinCount - (float)reducedSpectreData.size()) - 0) / ((float)config->visibleSpectreBinCount - 0) * (1.5f - 0.1f) + 0.1f;
			//float thickness = 1.5f;
			//(value - From1) / (From2 - From1) * (To2 - To1) + To1;
			// 
			// from1 .. from2 = [0, config->visibleSpectreBinCount]
			// to1 .. to2 = [1.0f, 3.0f]
			// value => [0, reducedSpectreData.size()]

			if (config->spectre.contourShowsPower) {
				Waterfall::RGB powerRGB = waterfall->getColorForPowerInSpectre(reducedSpectreData[i], viewModel->waterfallMin * config->spectre.topCoeff, viewModel->waterfallMax * config->spectre.bottomCoeff);
				draw_list->AddLine(lineX1, lineX2, IM_COL32(powerRGB.r, powerRGB.g, powerRGB.b, 255), thickness);
			}
			else {
				draw_list->AddLine(lineX1, lineX2, config->colorTheme.spectreProfileColor, thickness);
			}
		}
	}

	//auto end = std::chrono::steady_clock::now();
	//auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	//std::cout << "The time: " << elapsed_ms.count() << " micros\n";

	//reducedSpectreData.clear();
}

void Spectre::drawFreqPointerMark(ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, ImDrawList* draw_list, ReceiverLogic* receiverLogic) {
	ImGuiIO& io = ImGui::GetIO();
	
	int baseX = 20;
	int baseY = 20;
	int baseWidth = 130;
	int baseHeight = 40;

	if (!viewModel->mouseBusy && 
		isMouseOnRegion(Region{ ImVec2(startWindowPoint.x + sWD.rightPadding, startWindowPoint.y), ImVec2(startWindowPoint.x + windowLeftBottomCorner.x - sWD.leftPadding, startWindowPoint.y + windowLeftBottomCorner.y) } ) == true &&
		io.MousePos.y < startWindowPoint.y + windowLeftBottomCorner.y - baseHeight - 10) {


		ImVec2 lineX1(
			io.MousePos.x + baseX,
			io.MousePos.y + baseY
		);
		ImVec2 lineX2(
			io.MousePos.x + baseX + baseWidth,
			io.MousePos.y + baseY
		);

		ImVec2 lineX3(
			io.MousePos.x + baseX + baseWidth,
			io.MousePos.y + baseY + baseHeight
		);

		ImVec2 lineX4(
			io.MousePos.x + baseX,
			io.MousePos.y + baseY + baseHeight
		);

		std::unique_ptr<ImVec2[]> polygonPtr(new ImVec2[]{ lineX1 , lineX2 , lineX3 , lineX4 });

		draw_list->AddConvexPolyFilled(polygonPtr.get(), 4, BASE_COLOR);

		//delete [] polygon;

		ImGui::SetCursorPos(ImVec2(io.MousePos.x - (startWindowPoint.x + sWD.rightPadding) + 100, io.MousePos.y - startWindowPoint.y + 62));
		ImGui::Text(Utils::getPrettyFreq((int)receiverLogic->getFreqByPosOnSpectrePx(io.MousePos.x - (startWindowPoint.x + sWD.rightPadding))).c_str());
		ImGui::SetCursorPos(ImVec2(0, 0));
	}
}

void Spectre::drawMemoryMarks(ImDrawList* draw_list, FlowingSpectre* flowingSpec, ReceiverLogic* receiverLogic) {
	const int MARK_HEIGHT_INDENT = -30;
	auto range = flowingSpec->getVisibleFreqsRangeAbsolute();
	for (int i = 0; i < config->memoryVector.size(); i++) {
		auto posX = receiverLogic->getPosOnSpectreByFreq(config->memoryVector[i].freq);
		if (posX > -1) {
			string mark; 
			mark.append("[").append(to_string(i)).append("]");
			int x = posX + sWD.leftPadding;
			int y = spectreHeight + MARK_HEIGHT_INDENT;
			/*draw_list->AddText(
				ImVec2(x, y),
				GREEN,
				mark.c_str()
			);*/
			ImGui::SetCursorPos(ImVec2(x, y));
			ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(GREEN), mark.c_str());
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
				string tooltipText("Description: " + config->memoryVector[i].desc + "\nFreq: " + Utils::getPrettyFreq(config->memoryVector[i].freq) + " Hz\nClick to go");
				disableControlForID(100 * i);
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(tooltipText.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
				if (ImGui::IsItemClicked()) {
					waterfall->clear();
					executeMemoryRecord(config->memoryVector[i], receiverLogic);
				}
			} else enableControlForID(100 * i);
			ImGui::SetCursorPos(ImVec2(0, 0));
		}
	}
}

void Spectre::executeMemoryRecord(Config::MemoryRecord record, ReceiverLogic* receiverLogic) {
	waterfall->clear();
	receiverLogic->setFreq(record.freq);
	viewModel->receiverMode = record.modulation;
	viewModel->filterWidth = record.filterWidth;
}