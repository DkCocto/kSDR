#include "CircleBufferWriterThread.h"

CircleBufferWriterThread::CircleBufferWriterThread(Config* config, CircleBuffer* cb, SoundCard* sc) {
	this->config = config;
	this->soundWriterCircleBuffer = cb;
	this->soundCard = sc;
	len = config->bufferWriteAudioLen;
}

void CircleBufferWriterThread::run() {

	ViewModel* viewModel = Display::instance->viewModel;

	float secondsInBuffer = 0.0;

	while (true) {

		/*if (!config->device->status->isOK) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}*/

		int available = soundWriterCircleBuffer->available();
		secondsInBuffer = (float)available / config->outputSamplerate;
		
		//Если начинается задержка в звуке, то сбросить буффер. Это ликвидирует задержку в звуке.
		if (secondsInBuffer > 0.3) {
			soundWriterCircleBuffer->reset();
			continue;
		}

		if (available >= len) {
			float* data = soundWriterCircleBuffer->read(len);
			soundCard->write(data, len);
			delete data;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

std::thread CircleBufferWriterThread::start() {
	std::thread p(&CircleBufferWriterThread::run, this);
	return p;
}