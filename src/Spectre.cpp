#include "Spectre.h"
#include "string"
#include "vector"

#define GRAY						IM_COL32(95, 95, 95, 255)
#define SPECTRE_FREQ_MARK_COUNT		20
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
	receiverLogicNew = new ReceiverLogicNew(config);
	maxdBKalman = new KalmanFilter(1, 0.005);
	ratioKalman = new KalmanFilter(10, 0.0001);
}

int savedStartWindowX = 0;
int savedEndWindowX = 0;
int savedSpectreWidthInPX = 1;

float veryMinSpectreVal = 0;
float veryMaxSpectreVal = -1000;

long countFrames = 0;


//#define checkImageWidth 128
//#define checkImageHeight 128



void Spectre::draw() {
	ImGui::Begin("Spectre");
		ImGuiIO& io = ImGui::GetIO();
		//ImDrawList* draw_list = ImGui::GetWindowDrawList();

		int rightPadding = 40;
		int leftPadding = 40;
		int waterfallPaddingTop = 50;

		//Начальная точка окна
		ImVec2 startWindowPoint = ImGui::GetCursorScreenPos();

		//Нижняя левая точка окна
		ImVec2 windowLeftBottomCorner = ImGui::GetContentRegionAvail();
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
			int markCount = SPECTRE_FREQ_MARK_COUNT;
			int sampleRateStep = config->inputSamplerate / markCount;
			float stepInPX = (float)spectreWidthInPX / (float)markCount;
			//printf("%i\r\n", viewModel->frequency);
			//int step = viewModel->frequency - config->inputSamplerate / 2;
			for (int i = 0; i <= markCount; i++) {
				draw_list->AddText(
					ImVec2(startWindowPoint.x + rightPadding + i * stepInPX - 20.0, startWindowPoint.y + spectreHeight + 10.0), 
					IM_COL32_WHITE, 
					std::to_string((viewModel->frequency - config->inputSamplerate / 2) + i * sampleRateStep).c_str()
				);
					
				draw_list->AddLine(
					ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight),
					ImVec2(startWindowPoint.x + rightPadding + i * stepInPX, startWindowPoint.y + spectreHeight - 5.0), 
					IM_COL32_WHITE, 2.0f
				);
			}
			//-----------------------------
	
			fftSH->getSemaphore()->lock();

			float* spectreData = fftSH->getOutput();

			storeSignaldB(spectreData);

			int spectreSize = fftSH->getSpectreSize();

			MIN_MAX m = getMinMaxInSpectre(spectreData, spectreSize);

			if (veryMinSpectreVal > m.min) veryMinSpectreVal = m.min;
			if (veryMaxSpectreVal < m.max) veryMaxSpectreVal = m.max;


			float stepX = (windowLeftBottomCorner.x - rightPadding - leftPadding)  / (spectreSize);

			float ratio = ratioKalman->filter(spectreHeight / (abs(veryMinSpectreVal) - abs(veryMaxSpectreVal)));

			float kaka = startWindowPoint.y - abs(veryMinSpectreVal) * ratio;
			float koeff = 0;
			if (abs(veryMinSpectreVal) * ratio > spectreHeight) {
				koeff = ( - 1) *  (abs(veryMinSpectreVal)* ratio - spectreHeight);
			} else {
				koeff = spectreHeight - abs(veryMinSpectreVal) * ratio;
			}
				
			for (int i = 0; i < spectreSize - 1; i++) {
				ImVec2 lineX1(startWindowPoint.x + (i * stepX) + rightPadding, startWindowPoint.y - spectreData[fftSH->getTrueBin(i)] * ratio + koeff);
				ImVec2 lineX2(startWindowPoint.x + ((i + 1) * stepX) + rightPadding, startWindowPoint.y - spectreData[fftSH->getTrueBin(i + 1)] * ratio + koeff);
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 2.0f);
			}

			//dB mark line
			stepInPX = (float)spectreHeight / (float)SPECTRE_DB_MARK_COUNT;
			float stepdB = (abs(veryMinSpectreVal) - abs(veryMaxSpectreVal)) / SPECTRE_DB_MARK_COUNT;

			for (int i = 0; i < SPECTRE_DB_MARK_COUNT; i++) {
				draw_list->AddText(
					ImVec2(startWindowPoint.x + rightPadding - 35, startWindowPoint.y + spectreHeight - i * stepInPX - 15),
					IM_COL32_WHITE, 
					std::to_string((int)round(veryMinSpectreVal + i * stepdB)).c_str()
				);
			}

			//---------------

			fftSH->getSemaphore()->unlock();

			//Выполняется если Нажатие мышки произошло внутри спектра
			if (ImGui::IsMouseClicked(0) && isMouseOnSpectreRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + spectreHeight)) {
				receiverLogicNew->saveDelta(io.MousePos.x - (startWindowPoint.x + rightPadding));
			}

			//Выполняется если Нажатие и удержание мышки произошло внутри спектра
			if (ImGui::IsMouseDown(0) && isMouseOnSpectreRegion(startWindowPoint.x + rightPadding, startWindowPoint.y, startWindowPoint.x + windowLeftBottomCorner.x - leftPadding, startWindowPoint.y + spectreHeight)) {
				//ImGui::Text("Pipka: %g %g", io.MousePos.x, io.MousePos.y);
				receiverLogicNew->setPosition(io.MousePos.x - (startWindowPoint.x + rightPadding), spectreWidthInPX);
			}

			//Определяем сдвинулось ли окно по x
			if (savedStartWindowX != startWindowPoint.x) {
				savedStartWindowX = startWindowPoint.x;
			}

			//Определяем изменился ли размер окна по x
			if (savedEndWindowX != windowLeftBottomCorner.x) {
				receiverLogicNew->update(savedSpectreWidthInPX, spectreWidthInPX);
				savedSpectreWidthInPX = spectreWidthInPX;
				savedEndWindowX = windowLeftBottomCorner.x;
			}

			//receive region
			draw_list->AddLine(
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), 0),
				ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y + spectreHeight),
				GRAY, 2.0f);


			float delta = receiverLogicNew->getFilterWidthAbs(viewModel->filterWidth);

			switch (viewModel->receiverMode) {
			case USB:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() + delta, startWindowPoint.y + spectreHeight),
					IM_COL32(95, 95, 95, 125), 0);

				break;
			case LSB:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition(), startWindowPoint.y + spectreHeight),
					IM_COL32(95, 95, 95, 125), 0);

				break;
			case AM:

				draw_list->AddRectFilled(
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() - delta, startWindowPoint.y - 10),
					ImVec2(startWindowPoint.x + rightPadding + receiverLogicNew->getPosition() + delta, startWindowPoint.y + spectreHeight),
					IM_COL32(95, 95, 95, 125), 0);

				break;
			}
			//---

		ImGui::EndChild();

		ImGui::BeginChild("Waterfall", ImVec2(ImGui::GetContentRegionAvail().x, windowLeftBottomCorner.y - spectreHeight - 5), false, ImGuiWindowFlags_NoMove);
			
			int waterfallLineHeight = 2;
			
			if (countFrames % 2 == 0) {
				waterfall->setMinMaxValue(m.min, m.max);
				waterfall->putData(fftSH, spectreData, waterfallLineHeight);
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

		ImGui::EndChild();

	ImGui::End();
	countFrames++;
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