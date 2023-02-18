#include "Env.h"
#include "Config.h"

#include "SoundCard.h"
#include "CircleBuffer.h"

#include "Display.h"
#include "RSP1.h"
#include "Hackrf.h"
#include "SoundProcessorThread.h"
#include "CircleBufferWriterThread.h"
#include "RSPv2.h"

//Config* config = new Config(375000, 8, 4);
//Config* config = new Config(1000000, 2, 8);
//Config* config = new Config(500000, 4, 16);
Config* config = new Config();

SoundCard soundCard(config);

//Буфер для сигналов I Q
CircleBuffer* iqSignalsCircleBuffer = new CircleBuffer(config->circleBufferLen);

CircleBuffer* soundWriterCircleBuffer = new CircleBuffer(config->circleBufferLen);

FFTSpectreHandler* fftSpectreHandler = new FFTSpectreHandler(config);

//Поток берет данные из iqSignalsCircleBuffer обрабатывает их и размещается в буфер soundWriterCircleBuffer
SoundProcessorThread* soundProcessor = new SoundProcessorThread(config, iqSignalsCircleBuffer, soundWriterCircleBuffer, fftSpectreHandler);

//Поток берет данные из soundWriterCircleBuffer и отдаёт их на воспроизведение в звуковую плату
CircleBufferWriterThread* circleBufferWriterThread = new CircleBufferWriterThread(config, soundWriterCircleBuffer, &soundCard);
//

//Создаем объект дисплей
Display* display = new Display(config, fftSpectreHandler);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {

	switch (config->deviceType) {
		//RSP
		case 0:
			config->device = new RSP1(config, iqSignalsCircleBuffer);
			break;
		//Hackrf
		case 1:
			config->device = new Hackrf(config, iqSignalsCircleBuffer);
			break;
	}

	//Инициализируем звуковую карту
	soundCard.open();

	//Инициализируем устройство
	switch (config->deviceType) {
		case Config::RSP:
			((RSP1*)config->device)->init();
			break;
		case Config::HACKRF:
			((Hackrf*)config->device)->init();
			break;
	}

	//RSPv2* rspv2 = new RSPv2();
	//rspv2->init();

	fftSpectreHandler->start().detach();
	circleBufferWriterThread->start().detach();
	soundProcessor->start().detach();

	display->init();
	display->mainLoop();

	delete display;
	delete config;
}