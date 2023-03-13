#include "Env.h"
#include "Config.h"
#include "SpeedCount.h"

#include "SoundCard.h"
#include "CircleBuffer.h"

#include "Display.h"
#include "RSP1.h"
#include "Hackrf.h"
#include "RTLDevice.h"
#include "SoundProcessorThread.h"
#include "CircleBufferWriterThread.h"
#include "RSPv2.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
// 
//Создаем объект дисплей
Display* display = new Display(config, fftSpectreHandler);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

#include "FastMath.h"

int main() {

	switch (config->deviceType) {
		//RSP
		case 0:
			if (config->rsp.api == 3) {
				config->device = new RSPv2(config, iqSignalsCircleBuffer);
				break;
			}
			config->device = new RSP1(config, iqSignalsCircleBuffer);
			break;
		//Hackrf
		case 1:
			config->device = new Hackrf(config, iqSignalsCircleBuffer);
			break;
		//RTL
		case 2:
			config->device = new RTLDevice(config, iqSignalsCircleBuffer);
			break;
	}

	//Инициализируем устройство
	switch (config->deviceType) {
		case Config::RSP:
			if (config->rsp.api == 3) {
				((RSPv2*)config->device)->init();
				break;
			}
			((RSP1*)config->device)->init();
			break;
		case Config::HACKRF:
			((Hackrf*)config->device)->init();
			break;
		case Config::RTL:
			((RTLDevice*)config->device)->init();
			break;
	}

	//Инициализируем звуковую карту
	soundCard.open();

	fftSpectreHandler->start().detach();
	circleBufferWriterThread->start().detach();
	soundProcessor->start().detach();

	display->init();
	display->mainLoop();

	delete display;
	delete config;
}