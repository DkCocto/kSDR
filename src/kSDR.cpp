#include "Env.h"
#include "Environment.h"
#include "Config.h"

#include "Display.h"
#include "SoundProcessorThread.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Environment env;

//Создаем объект дисплей
Display* display = new Display(&env);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {
	/*std::vector<int> pipa = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> kaka(pipa.size());

	std::copy(pipa.begin() + pipa.size() / 2, pipa.end(), kaka.begin());
	std::copy(pipa.begin(), pipa.end() - pipa.size() / 2, kaka.begin() + pipa.size() / 2);

	for (int i = 0; i < kaka.size(); i++) {
		printf("%d ", kaka[i]);
	}
	exit(0);*/

	env.startProcessing();

	display->init();
	display->mainLoop();

	//env.stopProcessing();

	delete display;
}