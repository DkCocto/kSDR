#include "SoundReaderThread.h"

SoundReaderThread::SoundReaderThread(Config* config, SoundCard* sc, CircleBuffer* cb) {
	this->config = config;
	soundCard = sc;
	circleBuffer = cb;
}

void SoundReaderThread::run() {
	int len = config->audioReadFrameLen * config->inputChannelNumber;
	float* buffer = new float[len];
	while (true) {
		//�ircleBuffer->write(soundCard->read(buffer, len), config->audioReadFrameLen * config->inputChannelNumber);
	}
}

std::thread SoundReaderThread::start() {
	std::thread p(&SoundReaderThread::run, this);
	return p;
}