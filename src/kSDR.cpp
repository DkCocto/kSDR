#include "Env.h"
#include "Config.h"

#include "SoundCard.h"
#include "CircleBuffer.h"

#include "Display.h"
#include "RSP1.h"
/////#include "SoundProcessorThread.h"
#include "CircleBufferReaderThread.h"
/////#include "RTLDeviceReaderThread.h"
#include "CircleBufferWriterThread.h"

//Config* config = new Config(375000, 8, 4);
//Config* config = new Config(1000000, 2, 8);
Config* config = new Config(500000, 4, 4);

SoundCard soundCard(config);

CircleBuffer* soundCardReadCircleBuffer = new CircleBuffer(config->circleBufferLen);

RSP1 rsp1(config, soundCardReadCircleBuffer);

CircleBuffer* soundProcessorCircleBuffer = new CircleBuffer(config->circleBufferLen);
CircleBuffer* soundWriterCircleBuffer = new CircleBuffer(config->circleBufferLen);
//
////////RTLDeviceReaderThread rtlDeviceReaderThread(soundCardReadCircleBuffer);
//

FFTSpectreHandler* fftSpectreHandler = new FFTSpectreHandler(config);
//
///////SoundReaderThread* soundReader = new SoundReaderThread(&soundCard, soundCardReadCircleBuffer);
//
///////ComPort c(soundCardReadCircleBuffer);
//
SoundProcessorThread* soundProcessor = new SoundProcessorThread(config, soundProcessorCircleBuffer, soundWriterCircleBuffer, fftSpectreHandler);
CircleBufferReaderThread* сircleBufferReaderThread = new CircleBufferReaderThread(config, soundCardReadCircleBuffer, soundProcessor);
CircleBufferWriterThread* circleBufferWriterThread = new CircleBufferWriterThread(config, soundWriterCircleBuffer, &soundCard);
//

//Создаем объект дисплей
Display* display = new Display(config, fftSpectreHandler);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;
int main() {
	rsp1.init();
	////exit(0);

	//Инициализируем звуковую карту
	soundCard.open();

	////rtlDeviceReaderThread.start().detach();

	////soundReader->start().detach();
	сircleBufferReaderThread->start().detach();
	circleBufferWriterThread->start().detach();
	soundProcessor->start().detach();

	display->init();
	display->mainLoop();
}