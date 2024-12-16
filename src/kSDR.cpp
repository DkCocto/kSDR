//#include "Environment.h"
#include "fftw/fftw3.h"
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

	/*cout << endl;

	for (int i = 0; i <= 255; i++) {
		cout << i << " = ";
		int v = i ^ (uint8_t)0x80;
		cout << v << ", ";
	}

	cot << endl;*/

	env.startProcessing();

	display->init();
	display->mainLoop();

	delete display;

}