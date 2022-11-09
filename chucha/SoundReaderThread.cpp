#include "SoundReaderThread.h"

SoundReaderThread::SoundReaderThread(SoundCard* sc, CircleBuffer* cb) {
	soundCard = sc;
	ñircleBuffer = cb;
}

void SoundReaderThread::run() {
	while (true) {
		ñircleBuffer->write(soundCard->read(), FRAMES_PER_BUFFER * NUM_INPUT_CHANNELS);
	}
}

std::thread SoundReaderThread::start() {
	std::thread p(&SoundReaderThread::run, this);
	return p;
}