#include "ReceiverRegionInterface.h"

ReceiverRegionInterface::ReceiverRegionInterface(SpectreWindowData* sWD, Config* config, ViewModel* viewModel, ReceiverLogicNew* receiverLogicNew) {
	this->config = config;
	this->viewModel = viewModel;
	this->sWD = sWD;
	this->receiverLogicNew = receiverLogicNew;
}

void ReceiverRegionInterface::drawRegion(ImDrawList* draw_list) {
	int receiverCenterLineX = sWD->startWindowPoint.x + sWD->rightPadding + receiverLogicNew->getPositionPX();

	draw_list->AddLine(
		ImVec2(receiverCenterLineX, sWD->startWindowPoint.y - 10),
		ImVec2(receiverCenterLineX, sWD->startWindowPoint.y + sWD->windowLeftBottomCorner.y + 10),
		GRAY, 2.0f);

	std::string freq = (Utils::getPrittyFreq((int)receiverLogicNew->getSelectedFreqNew())).append(" Hz");

	//freqTextX = (sWD->windowLeftBottomCorner.x - sWD->leftPadding - receiverPosAbsolute > 280) ? receiverPosAbsolute + 50 : receiverPosAbsolute - 300;
	freqTextX = sWD->startWindowPoint.x + sWD->rightPadding + 80;
	freqTextY = sWD->startWindowPoint.y + 50;

	ImGui::PushFont(viewModel->fontBigRegular);
	draw_list->AddText(
		ImVec2(
			freqTextX,
			freqTextY
		),
		IM_COL32_WHITE,
		freq.c_str()
	);
	ImGui::PopFont();

	float delta = receiverLogicNew->getFilterWidthAbs(viewModel->filterWidth);

	int receiverPosX = sWD->startWindowPoint.x + sWD->rightPadding + receiverLogicNew->getPositionPX();
	int receiverPosYTop = sWD->startWindowPoint.y - 10;
	int receiverPosYBottom = sWD->startWindowPoint.y + sWD->windowLeftBottomCorner.y + 10;

	switch (viewModel->receiverMode) {
	case USB:
		// Y!!!  spectreHeight + waterfallPaddingTop
		draw_list->AddRectFilled(
			ImVec2(receiverPosX, receiverPosYTop),
			ImVec2(receiverPosX + delta, receiverPosYBottom),
			config->colorTheme.receiveRegionColor, 0);

		break;
	case LSB:

		draw_list->AddRectFilled(
			ImVec2(receiverPosX - delta, receiverPosYTop),
			ImVec2(receiverPosX, receiverPosYBottom),
			config->colorTheme.receiveRegionColor, 0);

		break;
	case AM:

		draw_list->AddRectFilled(
			ImVec2(receiverPosX - delta, receiverPosYTop),
			ImVec2(receiverPosX + delta, receiverPosYBottom),
			config->colorTheme.receiveRegionColor, 0);

		break;
	}

	markDigitByMouse(draw_list);
}

int ReceiverRegionInterface::getFreqTextWidth() {
	return freqTextWidth;
}

int ReceiverRegionInterface::getFreqTextHeight() {
	return freqTextHeight;
}

int ReceiverRegionInterface::getFreqTextX() {
	return freqTextX;
}

int ReceiverRegionInterface::getFreqTextY() {
	return freqTextY;
}

int ReceiverRegionInterface::getSelectedDigit() {
	return selectedDigit;
}

bool ReceiverRegionInterface::isDigitSelected() {
	return (selectedDigit > 0);
}

void ReceiverRegionInterface::setupNewFreq(bool positive) {
	double sign = (positive == true) ? 1 : -1;
	
	int delta = pow(10, selectedDigit - 1);
	
	receiverLogicNew->setFreq((double)receiverLogicNew->getSelectedFreqNew() + sign * delta);
}

bool ReceiverRegionInterface::markDigitByMouse(ImDrawList* draw_list) {

	ImGuiIO& io = ImGui::GetIO();

	int totalDigits = to_string((int)receiverLogicNew->getSelectedFreqNew()).size();
	int totalDigitsWithDots = Utils::getPrittyFreq((int)receiverLogicNew->getSelectedFreqNew()).size();

	selectedDigit = -1;

	for (int i = 1; i <= totalDigits; i++) {
		int p = (i % 3 == 0) ? 1 : 0;
		int adding = (i / 3) - p; // 2 -> 0; 3 -> 0; 5 -> 1; 6 -> 1; 9 -> 2 

		int X = freqTextX + (totalDigitsWithDots - (i + adding)) * freqCharWidth;
		int Y = freqTextY + 5;

		if (
			io.MousePos.x >= X &&
			io.MousePos.x <= X + freqCharWidth &&
			io.MousePos.y >= freqTextY &&
			io.MousePos.y <= freqTextY + freqTextHeight
			) {
			selectedDigit = i;

			ImGui::PushFont(viewModel->fontBigRegular);
			draw_list->AddText(ImVec2(X, Y), IM_COL32_WHITE, "_");
			ImGui::PopFont();

			break;
		}

	}
	return selectedDigit > 0;
}