#include "SMeter.h"

#define YELLOW	IM_COL32(255, 233, 0, 255)
#define GREEN	IM_COL32(0, 204, 0, 255)
#define RED		IM_COL32(255, 7, 0, 255)
#define WHITE	IM_COL32(255, 255, 255, 255)

#define LEVEL_THICKNESS 10
#define LEVEL_PADDING_TOP 5
#define LEVEL_PADDING_BOTTOM 5

void SMeter::drawGrid(ImDrawList* draw_list) {
	float step = width / 15.0f;

	//¬ерхн€€ лини€ s метра часть бела€
	draw_list->AddLine(ImVec2(X, Y), ImVec2(X + step * 9.0f, Y), IM_COL32_WHITE, 1.0f);
	//¬ерхн€€ лини€ s метра часть где сигнал зашкаливает
	draw_list->AddLine(ImVec2(X + step * 9.0f, Y), ImVec2(X + width, Y), RED, 1.0f);

	
	//Ќижн€€ лини€ s метра часть бела€
	draw_list->AddLine(ImVec2(X, Y + LEVEL_THICKNESS + LEVEL_PADDING_TOP + LEVEL_PADDING_BOTTOM - 1), ImVec2(X + step * 9.0f, Y + LEVEL_THICKNESS + LEVEL_PADDING_BOTTOM + LEVEL_PADDING_TOP - 1), IM_COL32_WHITE, 1.0f);
	//Ќижн€€ лини€ s метра часть где сигнал зашкаливает
	draw_list->AddLine(ImVec2(X + step * 9.0f, Y + LEVEL_THICKNESS + LEVEL_PADDING_TOP + LEVEL_PADDING_BOTTOM - 1), ImVec2(X + width, Y + LEVEL_THICKNESS + LEVEL_PADDING_BOTTOM + LEVEL_PADDING_TOP - 1), RED, 1.0f);

	for (int i = 0; i <= 15; i++) {
		if (i == 0 || i % 2 != 0) {

			ImVec2 lineX1(X + i * step, Y);
			ImVec2 lineX2(X + i * step, Y - 8);

			if (i > 8) {
				draw_list->AddLine(lineX1, lineX2, RED, 3.0f);
			} else {
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 4.0f);
			}

			ImVec2 pos(X + i * step - 5, Y - 35);

			switch (i) {
			case(0): {
				draw_list->AddText(pos, GREEN, "S");
				break;
			}
			case(1): {
				draw_list->AddText(pos, GREEN, "1");
				break;
			}
			case(3): {
				draw_list->AddText(pos, GREEN, "3");
				break;
			}
			case(5): {
				draw_list->AddText(pos, GREEN, "5");
				break;
			}
			case(7): {
				draw_list->AddText(pos, GREEN, "7");
				break;
			}
			case(9): {
				draw_list->AddText(pos, GREEN, "9");
				break;
			}
			case(11): {
				draw_list->AddText(pos, RED, "+20");
				break;
			}
			case(13): {
				draw_list->AddText(pos, RED, "+40");
				break;
			}
			case(15): {
				draw_list->AddText(pos, RED, "+60");
				break;
			}
			default:
				break;
			}

		} else {
			ImVec2 lineX1(X + i * step, Y);
			ImVec2 lineX2(X + i * step, Y - 5);

			if (i > 8) {
				draw_list->AddLine(lineX1, lineX2, RED, 1.0f);
			} else {
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 1.0f);
			}

		}
	}
}

double storedLevel = 0.0;
double storedLevelDb = 0.0;

int count1 = 0;

auto color = WHITE;

void SMeter::drawLevel(ImDrawList* draw_list, FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic) {

	float dBValue = getSignaldB(spectreData, receiverLogic);

	if (dBValue <= -126.5) dBValue = -126.5;

	float levelVal = fromdBToLevel(dBValue);

	if (levelVal > 15.0f) levelVal = 15.0f;

	/*float step = width / 15.0f;

	if (levelVal >= 9) {
		draw_list->AddRectFilled(
			ImVec2(X, Y + LEVEL_PADDING_TOP), 
			ImVec2(X + step * 9, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS), 
			YELLOW, 0);

		draw_list->AddRectFilled(
			ImVec2(X + step * 9, Y + LEVEL_PADDING_TOP), 
			ImVec2(X + step * levelVal, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS),
			RED, 0);

	} else {
		ImVec2 lineX1(X, Y + LEVEL_PADDING_TOP);
		ImVec2 lineX2(X + step * levelVal, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS);
		draw_list->AddRectFilled(lineX1, lineX2, YELLOW, 0);
	}
	
	draw_list->AddLine(ImVec2(X + step * storedLevel, Y + LEVEL_PADDING_TOP - 1), ImVec2(X + step * storedLevel, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS - 1), color, 5.0f);
	
	*/

	//отступ между кубиками
	float otstup = 3.0f;
	//ширина кубика
	float cubikWidth = width / 30.0f - otstup;

	//old_range = old_max - old_min
	//new_range = new_max - new_min
	//converted = (((old - old_min) * new_range) / old_range) + new_min

	float delta = otstup + cubikWidth;

	for (int i = 0; i < 2 * levelVal - 1; i++) {
		float delta = otstup + cubikWidth;
		ImVec2 lineX1(X + otstup			  + i * delta, Y + LEVEL_PADDING_TOP);
		ImVec2 lineX2(X + otstup + cubikWidth + i * delta, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS);
		draw_list->AddRectFilled(lineX1, lineX2, (i >= 18) ? RED : YELLOW, 0);
	}

	/*if (round(storedLevel) <= 9) {
		color = WHITE;
	}
	else {
		color = RED;
	}*/

	ImVec2 lineX1(X + otstup + round((2 * storedLevel - 1 < 0) ? 0 : (2 * storedLevel - 1)) * delta, Y + LEVEL_PADDING_TOP);
	ImVec2 lineX2(X + otstup + cubikWidth + round((2 * storedLevel - 1 < 0) ? 0 : (2 * storedLevel - 1)) * delta, Y + LEVEL_PADDING_TOP + LEVEL_THICKNESS);
	draw_list->AddRectFilled(lineX1, lineX2, WHITE, 0);

	if (storedLevel <= 9) {
		color = WHITE;
	}
	else {
		color = RED;
	}

	std::string dBText = std::to_string((int)fromLevelToDb(storedLevel));

	draw_list->AddText(ImVec2(
		X - 40,
		Y
	), WHITE, dBText.c_str());

	draw_list->AddText(ImVec2(
		X + width + 12,
		Y
	), WHITE, getLevelDecodedString(fromLevelToDb(storedLevel)));

	int holdTimeSec = 4;
	float fps = 1000.0f / ImGui::GetIO().Framerate;

	if (count1 >= fps * holdTimeSec) {
		storedLevel -= 0.02;
	}
	
	if (storedLevel <= levelVal) {
		storedLevel = levelVal;
		count1 = 0;
	} else {
		count1++;
	}
}

float SMeter::fromdBToLevel(float dBValue) {
	//float levelVal = 0.1667f * dBValue + 21.1667f;
	float levelVal = 0.155f * dBValue + 19.817f;
	if (levelVal < 0.0f) levelVal = 0.0f;
	return levelVal;
}

float SMeter::fromLevelToDb(float level) {
	//float dBValue = (level - 21.1667f) / 0.1667f;
	float dBValue = 6.476f * level - 128.381f;
	return dBValue;
}

const char * SMeter::getLevelDecodedString(float dBValue) {
	int level = (int)fromdBToLevel(dBValue);

	if (level <= 1) return "S0";
	else if (level >= 1 && level < 2) return "S1";
	else if (level >= 2 && level < 3) return "S2";
	else if (level >= 3 && level < 4) return "S3";
	else if (level >= 4 && level < 5) return "S4";
	else if (level >= 5 && level < 6) return "S5";
	else if (level >= 6 && level < 7) return "S6";
	else if (level >= 7 && level < 8) return "S7";
	else if (level >= 8 && level < 9) return "S8";
	else if (level >= 9 && level < 10) return "S9";
	else return "S9+";
}

SMeter::SMeter(ViewModel* viewModel) {
	this->viewModel = viewModel;
	averageSignalDb = Average(5);
}

void SMeter::update(int X, int Y, int width, int height) {
	this->X = X;
	this->Y = Y;
	this->width = width;
	this->height = height;
}

void SMeter::draw(ImDrawList* draw_list, FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic) {
	drawGrid(draw_list);
	drawLevel(draw_list, spectreData, receiverLogic);
}

float SMeter::getSignaldB(FFTData::OUTPUT* spectreData, ReceiverLogic* receiverLogic) {
	ReceiverLogic::ReceiveBinArea r = receiverLogic->getReceiveBinsArea(viewModel->filterWidth, viewModel->receiverMode);
	float sum = 0.0;
	int len = r.B - r.A;
	if (len > 0) {

		float max = -1000.0;

		for (int i = r.A; i < r.B; i++) {
			if (spectreData->data[i] > max) {
				max = spectreData->data[i];
			}
		}
		return averageSignalDb.process(max);
	}
	return 0.0f;
}