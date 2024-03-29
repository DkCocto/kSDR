#include "CircleBufferWriterThread.h"

CircleBufferWriterThread::CircleBufferWriterThread(Config* config, DeviceController* deviceController, CircleBufferNew<float>* cb, SoundCard* sc) {
	this->config = config;
	this->deviceController = deviceController;
	this->soundWriterCircleBuffer = cb;
	soundWriterCircleBuffer->reset();
	this->soundCard = sc;
	len = config->bufferWriteAudioLen;
	data = new float[len];
}

CircleBufferWriterThread::~CircleBufferWriterThread() {
	delete[] data;
}

void CircleBufferWriterThread::run() {
	float secondsInBuffer = 0.0;

	isWorking_ = true;

	while (true) {
		if (!config->WORKING) {
			printf("CircleBufferWriterThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		int available = soundWriterCircleBuffer->available();
		secondsInBuffer = (float)available / config->outputSamplerate;
		
		//Если начинается задержка в звуке, то сбросить буффер. Это ликвидирует задержку в звуке.
		if (secondsInBuffer > 0.15) {
			soundWriterCircleBuffer->reset();
			continue;
		}

		if (available >= len) {
			soundWriterCircleBuffer->read(data, len);
			soundCard->write(data, len);
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

std::thread CircleBufferWriterThread::start() {
	std::thread p(&CircleBufferWriterThread::run, this);
	//DWORD result = ::SetThreadIdealProcessor(p.native_handle(), 3);
	//SetThreadPriority(p.native_handle(), THREAD_PRIORITY_HIGHEST);
	return p;
}
