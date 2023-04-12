#include "Environment.h"

#include "Display.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Environment env;

//Создаем объект дисплей
Display* display = new Display(&env);
//Инициализируем статическую переменную класса.
Display& d = *display;
Display* Display::instance = &d;

int main() {

	env.startProcessing();

	display->init();
	display->mainLoop();

	delete display;

}