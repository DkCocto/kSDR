#include "CircleBufferWriterThread.h"

CircleBufferWriterThread::CircleBufferWriterThread(Config* config, CircleBuffer* cb, SoundCard* sc) {
	this->config = config;
	this->soundWriterCircleBuffer = cb;
	this->soundCard = sc;
	len = config->bufferWriteAudioLen;
}

void CircleBufferWriterThread::run() {
	while (true) {
		int available = soundWriterCircleBuffer->available();
		if (available >= len) {
			float* data = soundWriterCircleBuffer->read(len);
			soundCard->write(data, len);
			delete data;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

std::thread CircleBufferWriterThread::start() {
	std::thread p(&CircleBufferWriterThread::run, this);
	return p;
}