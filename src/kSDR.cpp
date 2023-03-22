#include "Env.h"
#include "Environment.h"
#include "Config.h"

#include "Display.h"
#include "RSP1.h"
#include "Hackrf.h"
#include "RTLDevice.h"
#include "SoundProcessorThread.h"
#include "RSPv2.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Environment env;

//Создаем объект дисплей
Display* display = new Display(env.getConfig(), env.getViewModel(), env.getDeviceController(), env.getFlowingSpectre(), env.getFFTSpectreHandler(), env.getReceiverLogicNew());
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {
	Config* config = env.getConfig();

	env.startProcessing();

	display->init();
	display->mainLoop();

	env.stopProcessing();

	delete display;
}