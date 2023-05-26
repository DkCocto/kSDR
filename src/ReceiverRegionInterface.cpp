#include "ReceiverRegionInterface.h"

ReceiverRegionInterface::ReceiverRegionInterface(SpectreWindowData* sWD, Config* config, ViewModel* viewModel) {
	this->config = config;
	this->viewModel = viewModel;
	this->sWD = sWD;
	smeter = make_unique<SMeter>(viewModel);
}

void ReceiverRegionInterface::drawRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic, FFTData::OUTPUT* spectreData) {
	backgroundX = sWD->startWindowPoint.x + sWD->rightPadding + X;
	backgroundY = sWD->startWindowPoint.y + Y;

	freqTextX = backgroundX + backgroundPadding + freqTextMarginLeft;
	freqTextY = backgroundY + backgroundPadding;

	drawReceiveRegion(draw_list, receiverLogic);

	drawBackground(draw_list);

	//S-meter
	smeter->update(backgroundX + backgroundPadding + smetreMargin, backgroundY + freqTextHeight + 65, backgroundWidth - 2 * (backgroundPadding + smetreMargin), 50);
	smeter->draw(draw_list, spectreData, receiverLogic);
	//-------

	std::string freq = (Utils::getPrittyFreq((int)receiverLogic->getSelectedFreqNew())).append(" Hz");

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

	markDigitByMouse(draw_list, receiverLogic);
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

void ReceiverRegionInterface::setupNewFreq(bool positive, ReceiverLogic* receiverLogic) {
	double sign = (positive == true) ? 1 : -1;
	
	int delta = pow(10, selectedDigit - 1);
	
	receiverLogic->setFreq((double)receiverLogic->getSelectedFreqNew() + sign * delta);
}

bool ReceiverRegionInterface::markDigitByMouse(ImDrawList* draw_list, ReceiverLogic* receiverLogic) {

	ImGuiIO& io = ImGui::GetIO();

	int totalDigits = to_string((int)receiverLogic->getSelectedFreqNew()).size();
	int totalDigitsWithDots = Utils::getPrittyFreq((int)receiverLogic->getSelectedFreqNew()).size();

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

void ReceiverRegionInterface::drawBackground(ImDrawList* draw_list) {
	ImVec2 pointX1(backgroundX, backgroundY);
	ImVec2 pointX2(backgroundX + backgroundWidth, backgroundY + backgroundHeight);

	draw_list->AddRectFilled(pointX1, pointX2, BASE_COLOR);
}

void ReceiverRegionInterface::drawReceiveRegion(ImDrawList* draw_list, ReceiverLogic* receiverLogic) {
	int receiverCenterLineX = sWD->startWindowPoint.x + sWD->rightPadding + receiverLogic->getPositionPX();

	draw_list->AddLine(
		ImVec2(receiverCenterLineX, sWD->startWindowPoint.y - 10),
		ImVec2(receiverCenterLineX, sWD->startWindowPoint.y + sWD->windowLeftBottomCorner.y + 10),
		RED, 2.0f);

	float delta = receiverLogic->getFilterWidthAbs(viewModel->filterWidth);

	int receiverPosX = sWD->startWindowPoint.x + sWD->rightPadding + receiverLogic->getPositionPX();
	int receiverPosYTop = sWD->startWindowPoint.y - 10;
	int receiverPosYBottom = sWD->startWindowPoint.y + sWD->windowLeftBottomCorner.y + 10;

	switch (viewModel->receiverMode) {
		case DSB:
			draw_list->AddRectFilled(
				ImVec2(receiverPosX - delta, receiverPosYTop),
				ImVec2(receiverPosX + delta, receiverPosYBottom),
				config->colorTheme.receiveRegionColor, 0);
			break;
		case USB:
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
		case nFM:

			draw_list->AddRectFilled(
				ImVec2(receiverPosX - delta, receiverPosYTop),
				ImVec2(receiverPosX + delta, receiverPosYBottom),
				config->colorTheme.receiveRegionColor, 0);

			break;
	}
}