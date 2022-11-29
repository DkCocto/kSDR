#include "Spectre.h"
#include "string"
#include "vector"

#define GRAY						IM_COL32(95, 95, 95, 255)
#define BLUE						IM_COL32(27, 27, 179, 60)
#define GREEN						IM_COL32(0, 204, 0, 20)
#define SPECTRE_FREQ_MARK_COUNT		20
#define SPECTRE_FREQ_MARK_COUNT_DIV	10
#define SPECTRE_DB_MARK_COUNT		10

//Upper right (spectreX1, spectreY1), down left (spectreX2, spectreY2)
bool Spectre::isMouseOnSpectreRegion(int spectreX1, int spectreY1, int spectreX2, int spectreY2) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.MousePos.x >= spectreX1 && io.MousePos.x <= spectreX2 && io.MousePos.y >= spectreY1 && io.MousePos.y <= spectreY2) return true;
	return false;
}

Spectre::Spectre(Config* config, ViewModel* viewModel, FFTSpectreHandler* fftSH, double width, double height) {
	this->waterfall = new Waterfall(config, fftSH);
	//waterfall->start().detach();
	this->config = config;
	this->viewModel = viewModel;
	this->width = width;
	this->height = height;
	this->fftSH = fftSH;
	receiverLogicNew = new ReceiverLogicNew(config, viewModel);
	maxdBKalman = new KalmanFilter(1, 0.005);
	ratioKalman = new KalmanFilter(1, 0.001);
	spectreTranferKalman = new KalmanFilter(1, 0.001);
}

int savedStartWindowX = 0;
int savedEndWindowX = 0;
int savedSpectreWidthInPX = 1;

float veryMinSpectreVal = -100;
float veryMaxSpectreVal = -60;

long countFrames = 0;

void Spectre::draw() {

	float* spectreData = fftSH->getOutputCopy();

	ImGui::Begin("Spectre");
		ImGuiIO& io = ImGui::GetIO();
		//ImDrawList* draw_list = ImGui::GetWindowDrawList();

		int rightPadding = 40;
		int leftPadding = 40;
		int waterfallPaddingTop = 50;

		//Ќачальна€ точка окна
		ImVec2 startWindowPoint = ImGui::GetCursorScreenPos();

		//Ќижн€€ лева€ точка окна
		ImVec2 windowLeftBottomCorner = ImGui::GetContentRegionAvail();

		windowFrame.UPPER_RIGHT = startWindowPoint;
		windowFrame.BOTTOM_LEFT = ImVec2(startWindowPoint.x + windowLeftBottomCorner.x, startWindowPoint.y + windowLeftBottomCorner.y);

		int spectreHeight = windowLeftBottomCorner.y / 2.0;

		int spectreWidthInPX = windowLeftBottomCorner.x - rightPadding - leftPadding;

		//receiverLogicNew->setSpectrePositionX(startWindowPoint.x + rightPadding, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::BeginChild("Spectre1", ImVec2(ImGui::GetContentRegionAvail().x, spectreHeight), false, ImGuiWindowFlags_NoMove);
			
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
			int markCount = SPECTRE_FREQ_MARK_COUNT * SPECTRE_FREQ_MARK_COUNT_DIV;
			int sampleRateStep = (float)config->inputSamplerate / (float)markCount;
			float stepInPX = (float)spectreWidthInPX / (float)markCount;

			for (int i = 0; i <= markCount; i++) {
				if (i % SPECTRE_FREQ_MARK_COUNT_DIV == 0) {
					draw_list->AddText(
						ImVec2(startWindowPoint.x + rightPadding + i * stepInPX - 20.0, startWindowPoint.y + spectreHeight + 10.0),
						IM_COL32_WHITE,
						std::to_string((viewModel->centerFrequency - (config->inputSamplerate / 2)) + i * sampleRateStep).c_str()
					);
					draw_list->AddLine(
						ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
						ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 9.0),
						IM_COL32_WHITE, 2.0f
					);
				} else {
					draw_list->AddLine(
						ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 2),
						ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 5.0),
						IM_COL32_WHITE, 2.0f
					);
				}
			}
			//-----------------------------


			storeSignaldB(spectreData);

			int spectreSize = fftSH->getSpectreSize();

			MIN_MAX m = getMinMaxInSpectre(spectreData, spectreSize);

			if (veryMinSpectreVal > m.min) veryMinSpectreVal = m.min;
			if (veryMaxSpectreVal < m.max) veryMaxSpectreVal = m.max;

			float stepX = (windowLeftBottomCorner.x - rightPadding - leftPadding)  / (spectreSize);

			float ratio = ratioKalman->filter((float)spectreHeight / (abs(veryMinSpectreVal) - abs(viewModel->maxDb)));

			//float kaka = startWindowPoint.y - abs(veryMinSpectreVal) * ratio;
			float koeff = 0;
			if (abs(veryMinSpectreVal) * ratio > (float)spectreHeight) {
				koeff = (-1.0) *  (abs(veryMinSpectreVal) * ratio - (float)spectreHeight);
			} else {
				koeff = (float)spectreHeight - abs(veryMinSpectreVal) * ratio;
			}
				
			koeff = spectreTranferKalman->filter(koeff);

			for (int i = 0; i < spectreSize - 1; i++) {
				ImVec2 lineX1(startWindowPoint.x + (i * stepX) + rightPadding, startWindowPoint.y - spectreData[fftSH->getTrueBin(i)] * ratio + koeff);
				ImVec2 lineX2(startWindowPoint.x + ((i + 1) * stepX) + rightPadding, startWindowPoint.y - spectreData[fftSH->getTrueBin(i + 1)] * ratio + koeff);
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 2.0f);
			}

			//dB mark line
			stepInPX = (float)spectreHeight / (float)SPECTRE_DB_MARK_COUNT;
			float stepdB = (abs(veryMinSpectreVal) - abs(viewModel->maxDb)) / SPECTRE_DB_MARK_COUNT;

			for (int i = 0; i < SPECTRE_DB_MARK_COUNT; i++) {
				draw_list->AddText(
					ImVec2(startWindowPoint.x + rightPadding - 35, startWindowPoint.y + spectreHeight - i * stepInPX - 15),
					IM_COL32_WHITE, 
					std::to_string((int)round(veryMinSpectreVal + i * stepdB)).c_str()
				);
			}

			//---------------

			
			if (!viewModel->mouseBusy && isMouseOnSpectreRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + windowLeftBottomCorner.y)) {
				//receiverLogicNew->saveDelta(0);
				//spectreWidthInPX
				receiverLogicNew->setFreq(viewModel->centerFrequency + receiverLogicNew->getSelectedFreq() + io.MouseWheel * 10);
				//receiverLogicNew->setPosition(receiverLogicNew->getPosition() + io.MouseWheel, true);
			}

			//¬ыполн€етс€ если Ќажатие мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
			if (!viewModel->mouseBusy && ImGui::IsMouseClicked(0) && isMouseOnSpectreRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + windowLeftBottomCorner.y)) {
				receiverLogicNew->saveDelta(io.MousePos.x - (startWindowPoint.x + rightPadding));
			}

			//¬ыполн€етс€ если Ќажатие и удержание мышки произошло внутри спектра и мышка не была где то уже нажата вне окна
			if (!viewModel->mouseBusy && ImGui::IsMouseDown(0) && isMouseOnSpectreRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + windowLeftBottomCorner.y)) {
				/*if (ImGui::IsMouseDown(1)) {
					receiverLogicNew->setPosition(receiverLogicNew->getPosition() + startWindowPoint.x + rightPadding + io.MousePos.x * 0.01, spectreWidthInPX, true);
				} else */
				//spectreWidthInPX
				receiverLogicNew->setPosition(io.MousePos.x - (startWindowPoint.x + rightPadding), false);
			}

			//ќпредел€ем сдвинулось ли окно по x
			if (savedStartWindowX != startWindowPoint.x) {
				savedStartWindowX = startWindowPoint.x;
			}

			//ќпредел€ем изменилс€ ли размер окна по x
			if (savedEndWindowX != windowLeftBottomCorner.x) {
				receiverLogicNew->update(savedSpectreWidthInPX, spectreWidthInPX);
				savedSpectreWidthInPX = spectreWidthInPX;
				savedEndWindowX = windowLeftBottomCorner.x;
			}

		ImGui::EndChild();

		ImGui::BeginChild("Waterfall", ImVec2(ImGui::GetContentRegionAvail().x, windowLeftBottomCorner.y - spectreHeight - 5), false, ImGuiWindowFlags_NoMove);
			
			int waterfallLineHeight = 1;
			
			if (countFrames % 1 == 0) {
				//waterfall->setMinMaxValue(m.min, m.max);
				waterfall->setMinMaxValue(viewModel->waterfallMin, viewModel->waterfallMax);
				waterfall->putData(fftSH, spectreData, waterfallLineHeight);
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
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y - 10),
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y + spectreHeight + waterfallPaddingTop),
				GRAY, 2.0f);

			std::string freq = std::to_string((int)(viewModel->centerFrequency + receiverLogicNew->getSelectedFreq()));
			const char* t2 = " Hz";

			char* s = new char[freq.length() + strlen(t2) + 1];
			strcpy(s, freq.c_str());
			strcat(s, t2);

			ImGui::PushFont(viewModel->fontBigRegular);
			draw_list->AddText(
				ImVec2(
					startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() + 20, 
					startWindowPoint.y + 10
				),
				IM_COL32_WHITE,
				s
			);
			ImGui::PopFont();

			float delta = receiverLogicNew->getFilterWidthAbs(viewModel->filterWidth);

			switch (viewModel->receiverMode) {
			case USB:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() + delta, startWindowPoint.y + spectreHeight + waterfallPaddingTop),
					BLUE, 0);

				break;
			case LSB:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y + spectreHeight + waterfallPaddingTop),
					BLUE, 0);

				break;
			case AM:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() + delta, startWindowPoint.y + spectreHeight + waterfallPaddingTop),
					BLUE, 0);

				break;
			}
			//---

		ImGui::EndChild();

	ImGui::End();
	countFrames++;

	delete[] spectreData;
}

void Spectre::storeSignaldB(float* spectreData) {
	ReceiverLogicNew::ReceiveBinArea r = receiverLogicNew->getReceiveBinsArea(viewModel->filterWidth, viewModel->receiverMode);

	//viewModel->serviceField1 = r.A;

	float sum = 0.0;
	int len = r.B - r.A;

	if (len > 0) {
		//Utils::printArray(spectre, 64);
		//printf("%i %i\r\n", r.A, r.B);

		float max = -1000.0;

		for (int i = r.A; i < r.B; i++) {
			if (spectreData[fftSH->getTrueBin(i)] > max) {
				max = spectreData[fftSH->getTrueBin(i)];
			}
			//sum += spectre[i];
		}
		viewModel->signalMaxdB = maxdBKalman->filter(max);
	}
}

Spectre::MIN_MAX Spectre::getMinMaxInSpectre(float* spectreData, int len) {
	float min = 1000.0;
	float max = -1000.0;
	for (int i = 0; i < len; i++) {
		if (spectreData[i] < min) min = spectreData[i];
		if (spectreData[i] > max) max = spectreData[i];
	}
	return MIN_MAX { min, max };
}