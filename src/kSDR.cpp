#include "Env.h"
#include "Environment.h"
#include "Config.h"

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

Environment environment;

SoundCard soundCard(&environment);

//Буфер для сигналов I Q
CircleBuffer* iqSignalsCircleBuffer = new CircleBuffer(environment.getConfig()->circleBufferLen);

CircleBuffer* soundWriterCircleBuffer = new CircleBuffer(environment.getConfig()->circleBufferLen);

FFTSpectreHandler* fftSpectreHandler = new FFTSpectreHandler(&environment);

//Поток берет данные из iqSignalsCircleBuffer обрабатывает их и размещается в буфер soundWriterCircleBuffer
SoundProcessorThread* soundProcessor = new SoundProcessorThread(environment.getConfig(), environment.getIQSourceBuffer(), soundWriterCircleBuffer, fftSpectreHandler);

//Поток берет данные из soundWriterCircleBuffer и отдаёт их на воспроизведение в звуковую плату
CircleBufferWriterThread* circleBufferWriterThread = new CircleBufferWriterThread(environment.getConfig(), soundWriterCircleBuffer, &soundCard);
//
//
//Создаем объект дисплей
Display* display = new Display(&environment, fftSpectreHandler);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {
	Config* config = environment.getConfig();
	/*switch (config->deviceType) {
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
	}*/

	//Инициализируем устройство
	/*switch (config->deviceType) {
		case DeviceType::RSP:
			if (config->rsp.api == 3) {
				((RSPv2*)config->device)->init();
				break;
			}
			((RSP1*)config->device)->init();
			break;
		case DeviceType::HACKRF:
			((Hackrf*)config->device)->init();
			break;
		case DeviceType::RTL:
			((RTLDevice*)config->device)->init();
			break;
	}*/

	//Инициализируем звуковую карту
	soundCard.open();

	fftSpectreHandler->start().detach();
	circleBufferWriterThread->start().detach();
	soundProcessor->start().detach();

	display->init();
	display->mainLoop();

	delete display;
}