#include "SMeter.h"
#include "Display.h"

#define YELLOW	IM_COL32(255, 233, 0, 255)
#define GREEN	IM_COL32(0, 204, 0, 255)
#define RED		IM_COL32(255, 7, 0, 255)

#define LEVEL_THICKNESS 10
#define LEVEL_PADDING_TOP 5
#define LEVEL_PADDING_BOTTOM 5

void SMeter::drawGrid(ImDrawList* draw_list) {
	double step = width / 15.0;

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available

	//draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	//draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	width = canvas_sz.x - 2 * x;

	//Верхняя линия s метра часть белая
	draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y + y), ImVec2(canvas_p0.x + width + x - step * 6, canvas_p0.y + y), IM_COL32_WHITE, 1.0f);
	//Верхняя линия s метра часть где сигнал зашкаливает
	draw_list->AddLine(ImVec2(canvas_p0.x + x + step * 9, canvas_p0.y + y), ImVec2(canvas_p0.x + width + x, canvas_p0.y + y), RED, 1.0f);

	//Нижняя линия s метра часть белая
	draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y + y + LEVEL_THICKNESS + LEVEL_PADDING_TOP + LEVEL_PADDING_BOTTOM), ImVec2(canvas_p0.x + width + x - step * 6, canvas_p0.y + y + LEVEL_THICKNESS + LEVEL_PADDING_BOTTOM + LEVEL_PADDING_TOP), IM_COL32_WHITE, 1.0f);
	//Нижняя линия s метра часть где сигнал зашкаливает
	draw_list->AddLine(ImVec2(canvas_p0.x + x + step * 9, canvas_p0.y + y + LEVEL_THICKNESS + LEVEL_PADDING_TOP + LEVEL_PADDING_BOTTOM), ImVec2(canvas_p0.x + width + x, canvas_p0.y + y + LEVEL_THICKNESS + LEVEL_PADDING_BOTTOM + LEVEL_PADDING_TOP), RED, 1.0f);

	for (int i = 0; i <= 15; i++) {
		if (i == 0 || i % 2 != 0) {

			ImVec2 lineX1(canvas_p0.x + x + i * step, canvas_p0.y + y);
			ImVec2 lineX2(canvas_p0.x + x + i * step, canvas_p0.y + y - 8);

			if (i > 8) {
				draw_list->AddLine(lineX1, lineX2, RED, 3.0f);
			} else {
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 3.0f);
			}

			ImVec2 pos(canvas_p0.x + x + i * step - 5, canvas_p0.y + y - 35);

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
			ImVec2 lineX1(canvas_p0.x + x + i * step, canvas_p0.y + y);
			ImVec2 lineX2(canvas_p0.x + x + i * step, canvas_p0.y + y - 5);

			if (i > 8) {
				draw_list->AddLine(lineX1, lineX2, RED, 1.0f);
			} else {
				draw_list->AddLine(lineX1, lineX2, IM_COL32_WHITE, 1.0f);
			}

		}
	}
}

void SMeter::drawLevel(ImDrawList* draw_list, double dBValue) {
	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!

	double levelVal = 0.1667 * dBValue + 21.1667;
	
	if (levelVal > 15) levelVal = 15.0;

	double step = width / 15;
	Display::instance->viewModel->serviceField1 = levelVal;

	if (levelVal >= 9) {
		draw_list->AddRectFilled(
			ImVec2(canvas_p0.x + x, canvas_p0.y + y + LEVEL_PADDING_TOP), 
			ImVec2(canvas_p0.x + x + step * 9, canvas_p0.y + y + LEVEL_PADDING_TOP + LEVEL_THICKNESS), 
			YELLOW, 0);

		draw_list->AddRectFilled(
			ImVec2(canvas_p0.x + x + step * 9, canvas_p0.y + y + LEVEL_PADDING_TOP), 
			ImVec2(canvas_p0.x + x + step * levelVal, canvas_p0.y + y + LEVEL_PADDING_TOP + LEVEL_THICKNESS),
			RED, 0);

	} else {
		ImVec2 lineX1(canvas_p0.x + x, canvas_p0.y + y + LEVEL_PADDING_TOP);
		ImVec2 lineX2(canvas_p0.x + x + step * levelVal, canvas_p0.y + y + LEVEL_PADDING_TOP + LEVEL_THICKNESS);
		draw_list->AddRectFilled(lineX1, lineX2, YELLOW, 0);
	}

}

SMeter::SMeter(double offsetX, double offsetY, double width, double height) {
	this->x = offsetX;
	this->y = offsetY;
	this->width = width;
	this->height = height;
}

void SMeter::draw(double dBValue) {
	ImGui::Begin("S-meter");
		//ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		drawGrid(draw_list);
		drawLevel(draw_list, dBValue);
	ImGui::End();
}