#include "Spectre.h"
#include "string"
#include "vector"

#define GRAY						IM_COL32(95, 95, 95, 255)
#define BLUE						IM_COL32(88, 88, 231, 255)
#define GREEN						IM_COL32(0, 204, 0, 80)

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
bool Spectre::isMouseOnRegion(int X1, int Y1, int X2, int Y2) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.MousePos.x >= X1 && io.MousePos.x <= X2 && io.MousePos.y >= Y1 && io.MousePos.y <= Y2) return true;
	return false;
}

void Spectre::waterfallAutoColorCorrection() {
	viewModel->waterfallMin = m.average - 12;
	viewModel->waterfallMax = m.max + 15;
}

void Spectre::spectreRatioAutoCorrection() {
	viewModel->minDb = m.average - 15;
	viewModel->ratio = m.max + 40;
}

Spectre::Spectre(Config* config, ViewModel* viewModel, FlowingFFTSpectre* flowingFFTSpectre) {
	//waterfall->start().detach();
	this->config = config;
	this->viewModel = viewModel;
	this->flowingFFTSpectre = flowingFFTSpectre;

	receiverLogicNew = new ReceiverLogicNew(config, viewModel, flowingFFTSpectre);

	maxdBKalman = new KalmanFilter(1, 0.005);
	ratioKalman = new KalmanFilter(1, 0.01);
	spectreTranferKalman = new KalmanFilter(1, 0.01);
	this->waterfall = new Waterfall(config, flowingFFTSpectre, viewModel);
}

int savedStartWindowX = 0;
int savedEndWindowX = 0;
int savedSpectreWidthInPX = 1;

float veryMinSpectreVal = -100;
float veryMaxSpectreVal = -60;

int countFrames = 0;

bool isFirstFrame = true;

void Spectre::draw() {

	receiverLogicNew->setCenterFrequency(viewModel->centerFrequency);

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Spectre");
		//Ќачальна¤ точка окна
		ImVec2 startWindowPoint = ImGui::GetCursorScreenPos();

		//Ќижн¤¤ лева¤ точка окна
		ImVec2 windowLeftBottomCorner = ImGui::GetContentRegionAvail();

		windowFrame.UPPER_RIGHT = startWindowPoint;
		windowFrame.BOTTOM_LEFT = ImVec2(startWindowPoint.x + windowLeftBottomCorner.x, startWindowPoint.y + windowLeftBottomCorner.y);

		int spectreHeight = windowLeftBottomCorner.y / 2.0;

		int spectreWidthInPX = windowLeftBottomCorner.x - rightPadding - leftPadding;

		handleEvents(startWindowPoint, windowLeftBottomCorner, spectreWidthInPX);

		float* fullSpectreData = flowingFFTSpectre->getSpectreHandler()->getOutputCopy(0, flowingFFTSpectre->getSpectreHandler()->getSpectreSize());

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::BeginChild("Spectre1", ImVec2(ImGui::GetContentRegionAvail().x, spectreHeight), false, ImGuiWindowFlags_NoMove);

		ImGui::SetCursorPos(ImVec2(spectreWidthInPX - 160, 0));
		if (ImGui::Button("Waterfall Auto")) waterfallAutoColorCorrection(); ImGui::SameLine();
		if (ImGui::Button("Spectre Auto")) spectreRatioAutoCorrection();
		ImGui::SetCursorPos(ImVec2(0, 0));

			storeSignaldB(fullSpectreData);

			int spectreSize = flowingFFTSpectre->getLen();

			std::vector<float> reducedSpectreData = flowingFFTSpectre->getReducedSpectre(
																	fullSpectreData, 
																	flowingFFTSpectre->getSpectreHandler()->getSpectreSize(), 
																	config->visibleSpectreBinCount
																	);

			m = getMinMaxInSpectre(reducedSpectreData, reducedSpectreData.size());

			//viewModel->serviceField1 = m.average;

			//if (veryMinSpectreVal > m.min) veryMinSpectreVal = m.min;
			veryMinSpectreVal = viewModel->minDb;
			viewModel->serviceField2 = viewModel->minDb;
			if (veryMaxSpectreVal < m.max) veryMaxSpectreVal = m.max;

			float stepX = (windowLeftBottomCorner.x - rightPadding - leftPadding) / reducedSpectreData.size();

			float ratio = ratioKalman->filter((float)spectreHeight / (abs(veryMinSpectreVal - viewModel->ratio)));

			float koeff = 0;
			if (abs(veryMinSpectreVal) * ratio > (float)spectreHeight) {
				koeff = (-1.0) * (abs(veryMinSpectreVal) * ratio - (float)spectreHeight);
			} else {
				koeff = (float)spectreHeight - abs(veryMinSpectreVal) * ratio;
			}

			koeff = spectreTranferKalman->filter(koeff);

			for (int i = 0; i < reducedSpectreData.size() - 1; i++) {

				float y1 = round(startWindowPoint.y - reducedSpectreData[i] * ratio + koeff);
				if (y1 >= startWindowPoint.y + spectreHeight) y1 = startWindowPoint.y + spectreHeight;

				float y2 = round(startWindowPoint.y - reducedSpectreData[i + 1] * ratio + koeff);
				if (y2 >= startWindowPoint.y + spectreHeight) y2 = startWindowPoint.y + spectreHeight;
			
				ImVec2 lineX1(
					startWindowPoint.x + round(i * stepX) + rightPadding, 
					y1
				);
				ImVec2 lineX2(
					startWindowPoint.x + round((i + 1) * stepX) + rightPadding,
					y2
				);

				ImVec2 lineX3(
					startWindowPoint.x + round((i + 1) * stepX) + rightPadding,
					startWindowPoint.y + spectreHeight
				);

				ImVec2 lineX4(
					startWindowPoint.x + round(i * stepX) + rightPadding,
					startWindowPoint.y + spectreHeight
				);

				ImVec2* polygon = new ImVec2[]{ lineX1 , lineX2 , lineX3 , lineX4 };

				draw_list->AddConvexPolyFilled(polygon, 4, config->colorTheme.spectreFillColor);
				delete[] polygon;

				draw_list->AddLine(lineX1, lineX2, config->colorTheme.spectreProfileColor, 2.0f);
			}
			
			reducedSpectreData.clear();

			//dB mark line
			float stepInPX = (float)spectreHeight / (float)SPECTRE_DB_MARK_COUNT;
			float stepdB = (abs(veryMinSpectreVal - viewModel->ratio)) / SPECTRE_DB_MARK_COUNT;

			for (int i = 0; i < SPECTRE_DB_MARK_COUNT; i++) {
				int dbMark = (int)round(veryMinSpectreVal + i * stepdB);
				if (dbMark <= 0) {
					draw_list->AddText(
						ImVec2(startWindowPoint.x + rightPadding - 35, startWindowPoint.y + spectreHeight - i * stepInPX - 15),
						IM_COL32_WHITE,
						std::to_string(dbMark).c_str()
					);
				}
			}
			//---------------
			
			drawFreqMarks(draw_list, startWindowPoint, windowLeftBottomCorner, spectreWidthInPX, spectreHeight);

		ImGui::EndChild();

		ImGui::BeginChild("Waterfall", ImVec2(ImGui::GetContentRegionAvail().x, windowLeftBottomCorner.y - spectreHeight - 5), false, ImGuiWindowFlags_NoMove);
			
			int waterfallLineHeight = 1;
			
			if (countFrames % 1 == 0) {
				//waterfall->setMinMaxValue(m.min, m.max);
				waterfall->setMinMaxValue(viewModel->waterfallMin, viewModel->waterfallMax);
				float* spectreData = flowingFFTSpectre->getData();
				waterfall->putData(spectreData, waterfallLineHeight);
				delete[] spectreData;
				countFrames = 0;
			}

			int waterfallHeight = windowLeftBottomCorner.y - spectreHeight - waterfallPaddingTop;

			stepX = spectreWidthInPX / (spectreSize / waterfall->getDiv());
			float stepY = (float)(waterfallHeight + waterfallPaddingTop) / (float)waterfall->getSize();

			for (int i = 0; i < waterfall->getSize(); i++) {
				draw_list->AddImage(
					(void*)(intptr_t)waterfall->getTexturesArray()[i], 
					ImVec2(
						startWindowPoint.x + rightPadding,
						startWindowPoint.y + waterfallHeight + 2 * waterfallPaddingTop + stepY * i
					), 
					ImVec2(
						startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, 
						startWindowPoint.y + waterfallHeight + 2 * waterfallPaddingTop + stepY * (i + 1)
					));
			}

			//receive region
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX(), startWindowPoint.y - 10),
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX(), startWindowPoint.y + windowLeftBottomCorner.y + 10),
				GRAY, 2.0f);

			std::string freq = (Utils::getPrittyFreq((int)receiverLogicNew->getSelectedFreqNew())).append(" Hz");

			ImGui::PushFont(viewModel->fontBigRegular);
			draw_list->AddText(
				ImVec2(
					startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX() + 20,
					startWindowPoint.y + 22
				),
				IM_COL32_WHITE,
				freq.c_str()
			);
			ImGui::PopFont();
			//delete[] s;

			float delta = receiverLogicNew->getFilterWidthAbs(viewModel->filterWidth);

			switch (viewModel->receiverMode) {
			case USB:
				// Y!!!  spectreHeight + waterfallPaddingTop
				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX(), startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX() + delta, startWindowPoint.y + windowLeftBottomCorner.y + 10),
					config->colorTheme.receiveRegionColor, 0);

				break;
			case LSB:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX(), startWindowPoint.y + windowLeftBottomCorner.y + 10),
					config->colorTheme.receiveRegionColor, 0);

				break;
			case AM:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPositionPX() + delta, startWindowPoint.y + windowLeftBottomCorner.y + 10),
					config->colorTheme.receiveRegionColor, 0);

				break;
			}
			//--
		ImGui::EndChild();

		drawFreqPointerMark(startWindowPoint, windowLeftBottomCorner, spectreWidthInPX, draw_list);

	ImGui::End();
	countFrames++;

	delete[] fullSpectreData;

	if (isFirstFrame) {
		receiverLogicNew->setFreq(config->lastSelectedFreq); //load last selected freq
		isFirstFrame = false;
	}
}

void Spectre::storeSignaldB(float* spectreData) {
	ReceiverLogicNew::ReceiveBinArea r = receiverLogicNew->getReceiveBinsArea(viewModel->filterWidth, viewModel->receiverMode);

	float sum = 0.0;
	int len = r.B - r.A;
	if (len > 0) {

		float max = -1000.0;

		for (int i = r.A; i < r.B; i++) {
			if (spectreData[i] > max) {
				max = spectreData[i];
			}
		}
		viewModel->signalMaxdB = maxdBKalman->filter(max);
	}
}

Spectre::MIN_MAX Spectre::getMinMaxInSpectre() {
	return m;
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
	return MIN_MAX{ min, max, sum / len };
}

void Spectre::handleEvents(ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX) {
	bool isMouseOnSpectre = isMouseOnRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + windowLeftBottomCorner.y);
	
	ImGuiIO& io = ImGui::GetIO();
	if (!viewModel->mouseBusy && isMouseOnSpectre) {

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
			if (!ctrlPressed) {
				int mouseWheelStep = 100;
				int selectedFreq = receiverLogicNew->getSelectedFreqNew();
				int selectedFreqShortByStep = selectedFreq / mouseWheelStep;
				if (mouseWheelVal > 0) receiverLogicNew->setFreq(mouseWheelStep * selectedFreqShortByStep + mouseWheelVal * mouseWheelStep);
				else {
					if (selectedFreq - selectedFreqShortByStep * mouseWheelStep > 0) {
						receiverLogicNew->setFreq(mouseWheelStep* selectedFreqShortByStep);
					}
					else {
						receiverLogicNew->setFreq(mouseWheelStep* selectedFreqShortByStep + mouseWheelVal * mouseWheelStep);
					}
				}
			}
			else {
				if (mouseWheelVal > 0) flowingFFTSpectre->zoomIn();
				else flowingFFTSpectre->zoomOut();
				receiverLogicNew->setFrequencyDelta(receiverLogicNew->getFrequencyDelta());
				waterfall->clear();
			}
		}
	}

	//¬ыполн¤етс¤ если Ќажатие мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
	if (!viewModel->mouseBusy && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && isMouseOnSpectre) {
		if (ImGui::IsMouseClicked(0)) {
			//receiverLogicNew->saveDelta(io.MousePos.x - (startWindowPoint.x + rightPadding));
			receiverLogicNew->saveSpectrePositionDelta(io.MousePos.x - (startWindowPoint.x + rightPadding));
		}
		if (ImGui::IsMouseClicked(1)) {
			flowingFFTSpectre->prepareForMovingSpectreByMouse(io.MousePos.x - (startWindowPoint.x + rightPadding));
		}
	}

	//¬ыполн¤етс¤ если удержание кнопки мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
	if (!viewModel->mouseBusy && (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1)) && isMouseOnSpectre) {
		if (ImGui::IsMouseDown(0)) {
			//receiverLogicNew->setPosition(io.MousePos.x - (startWindowPoint.x + rightPadding), false);
			receiverLogicNew->setFrequencyDeltaFromSavedPosition(io.MousePos.x - (startWindowPoint.x + rightPadding));
		}
		if (ImGui::IsMouseDown(1)) {

			float newCenterFreq = flowingFFTSpectre->moveSpectreByMouse(spectreWidthInPX, io.MousePos.x - (startWindowPoint.x + rightPadding));

			//viewModel->centerFrequency = round(newCenterFreq);
			//receiverLogicNew->setCenterFrequency(viewModel->centerFrequency);
			receiverLogicNew->setFrequencyDelta(receiverLogicNew->getFrequencyDelta());
			waterfall->clear();
		}
	}

	//ќпредел¤ем сдвинулось ли окно по x
	if (savedStartWindowX != startWindowPoint.x) {
		savedStartWindowX = startWindowPoint.x;
	}

	//ќпредел¤ем изменилс¤ ли размер окна по x
	if (savedEndWindowX != windowLeftBottomCorner.x) {
		receiverLogicNew->updateSpectreWidth(savedSpectreWidthInPX, spectreWidthInPX);
		savedSpectreWidthInPX = spectreWidthInPX;
		savedEndWindowX = windowLeftBottomCorner.x;
	}
}

void Spectre::drawFreqMarks(ImDrawList* draw_list, ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, int spectreHeight) {
	//Horizontal
	draw_list->AddLine(
		ImVec2(startWindowPoint.x + rightPadding, startWindowPoint.y + spectreHeight),
		ImVec2(startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + spectreHeight),
		GRAY, 4.0f);

	//Vertical
	draw_list->AddLine(
		ImVec2(startWindowPoint.x + rightPadding, 0),
		ImVec2(startWindowPoint.x + rightPadding, startWindowPoint.y + spectreHeight),
		GRAY, 2.0f);
	
	//Freqs mark line
	//Отметки частоты должны зависить от ширины окна спектра, подобрал такую зависимость
	int markCount = (int)(0.008 * windowLeftBottomCorner.x) * SPECTRE_FREQ_MARK_COUNT_DIV;
	
	FlowingFFTSpectre::FREQ_RANGE freqRange = flowingFFTSpectre->getVisibleFreqRangeFromSamplerate();

	float freqStep = (freqRange.second - freqRange.first) / (float)markCount;
	float stepInPX = (float)spectreWidthInPX / (float)markCount;

	for (int i = 0; i <= markCount; i++) {
		if (i % SPECTRE_FREQ_MARK_COUNT_DIV == 0) {
			//std::to_string().c_str()
			draw_list->AddText(
				ImVec2(startWindowPoint.x + rightPadding + i * stepInPX - 30.0, startWindowPoint.y + spectreHeight + 10.0),
				IM_COL32_WHITE,
				Utils::getPrittyFreq((int)(flowingFFTSpectre->getVisibleStartFrequency() + (float)i * freqStep)).c_str()
			);
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
				ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 9.0),
				IM_COL32_WHITE, 2.0f
			);
		}
		else {
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
				ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 5.0),
				IM_COL32_WHITE, 2.0f
			);
		}
	}
	//-----------------------------
}

void Spectre::drawFreqPointerMark(ImVec2 startWindowPoint, ImVec2 windowLeftBottomCorner, int spectreWidthInPX, ImDrawList* draw_list) {
	ImGuiIO& io = ImGui::GetIO();
	
	int baseX = 20;
	int baseY = 20;
	int baseWidth = 130;
	int baseHeight = 40;

	if (!viewModel->mouseBusy && 
		isMouseOnRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + windowLeftBottomCorner.y) == true &&
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

		ImVec2* polygon = new ImVec2[]{ lineX1 , lineX2 , lineX3 , lineX4 };

		draw_list->AddConvexPolyFilled(polygon, 4, BASE_COLOR);

		ImGui::SetCursorPos(ImVec2(io.MousePos.x - (startWindowPoint.x + rightPadding) + 105, io.MousePos.y - startWindowPoint.y + 62));
		ImGui::Text(Utils::getPrittyFreq((int)receiverLogicNew->getFreqByPosOnSpectrePx(io.MousePos.x - (startWindowPoint.x + rightPadding))).c_str());
		ImGui::SetCursorPos(ImVec2(0, 0));
	}
}
