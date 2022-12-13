#include "CircleBufferReaderThread.h"

CircleBufferReaderThread::CircleBufferReaderThread(Config* config, CircleBuffer* cb, SoundProcessorThread* sP) {
	this->config = config;
	ñircleBuffer = cb;
	soundProcessor = sP;
}

/*void CircleBufferReaderThread::run() {
	float* data;
	while (true) {
		int available = ñircleBuffer->available();
		if (available >= config->readSize) {
			float* data = ñircleBuffer->read(config->readSize);

			soundProcessor->putData(data, config->readSize);

			delete(data);
		} else {
			//printf("sdasda\r\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

std::thread CircleBufferReaderThread::start() {
	std::thread p(&CircleBufferReaderThread::run, this);
	return p;
}*/