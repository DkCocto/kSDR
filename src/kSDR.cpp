#include "Env.h"
#include "Config.h"

#include "SoundCard.h"
#include "CircleBuffer.h"

#include "Display.h"
#include "RSP1.h"
#include "Hackrf.h"
#include "SoundProcessorThread.h"
#include "CircleBufferWriterThread.h"

//Config* config = new Config(375000, 8, 4);
//Config* config = new Config(1000000, 2, 8);
//Config* config = new Config(500000, 4, 16);
Config* config = new Config(1000000);

SoundCard soundCard(config);

//Буфер для сигналов I Q
CircleBuffer* iqSignalsCircleBuffer = new CircleBuffer(config->circleBufferLen);

//CircleBuffer* soundProcessorCircleBuffer = new CircleBuffer(config->circleBufferLen);
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


//Поток берет данные из iqSignalsCircleBuffer обрабатывает их и размещается в буфер soundWriterCircleBuffer
SoundProcessorThread* soundProcessor = new SoundProcessorThread(config, iqSignalsCircleBuffer, soundWriterCircleBuffer, fftSpectreHandler);

//Поток берет данные из soundWriterCircleBuffer и отдаёт их на воспроизведение в звуковую плату
CircleBufferWriterThread* circleBufferWriterThread = new CircleBufferWriterThread(config, soundWriterCircleBuffer, &soundCard);
//

RSP1 rsp1(config, iqSignalsCircleBuffer);
//Инициализация устройства, а так же этот объект берет данные I Q и размещает их в буфере IQSignalsCircleBuffer
//Hackrf hackrf(config, iqSignalsCircleBuffer);

//Создаем объект дисплей
Display* display = new Display(config, fftSpectreHandler, NULL);
//Сразу же инициализируем статическую переменную класса. Она нужна для обработки событий.
Display& d = *display;
Display* Display::instance = &d;

int main() {
	//Инициализируем звуковую карту
	soundCard.open();

	////rtlDeviceReaderThread.start().detach();

	rsp1.init();
	//hackrf.init();

	////soundReader->start().detach();
	//сircleBufferReaderThread->start().detach();
	fftSpectreHandler->start().detach();
	circleBufferWriterThread->start().detach();
	soundProcessor->start().detach();

	display->init();
	display->mainLoop();

	delete display;
	delete config;
}