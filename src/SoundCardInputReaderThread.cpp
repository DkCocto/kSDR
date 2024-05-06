#include "SoundCardInputReaderThread.h"

SoundCardInputReaderThread::SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* soundCard) {
	this->config = config;
	this->soundInputBuffer = soundInputBuffer;
	this->soundCard = soundCard;

	audioFilter.init(audioFilter.LOWPASS, audioFilter.BARTLETT, 63, 3200, 0, config->inputSamplerateSound);
	resamplingFilter.init(resamplingFilter.LOWPASS, resamplingFilter.BARTLETT, 199, 8000, 0, config->currentWorkingInputSamplerate);
}

SoundCardInputReaderThread::~SoundCardInputReaderThread() { }

std::thread SoundCardInputReaderThread::start() {
	std::thread p(&SoundCardInputReaderThread::run, this);
	return p;
}

void SoundCardInputReaderThread::pause() {
	CurrentStatus = PAUSE;
}

void SoundCardInputReaderThread::continueRead() {
	CurrentStatus = START_READING;
}

void SoundCardInputReaderThread::run() {
	//float secondsInBuffer = 0.0;

	isWorking_ = true;

	const int BUFFER_READ_LEN = config->audioReadFrameLen;

	int relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound; // 4 000 000 / 40 000 = 100
	int upsamplingDataLen = BUFFER_READ_LEN * relation; // 8 * 100 = 800

	float* readBuffer = new float[BUFFER_READ_LEN];

	float* upsamplingData = new float[upsamplingDataLen];
	//memset(upsamplingData, 0, sizeof(float) * upsamplingDataLen);

	while (true) {
		if (!config->WORKING) {
			printf("SoundCardInputReaderThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (CurrentStatus == START_READING) {
			soundCard->startInput();
			CurrentStatus = READING;
		}

		if (CurrentStatus == PAUSE) {
			soundCard->stopInput();
			CurrentStatus = REST;
			continue;
		}

		if (CurrentStatus == REST) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		if (soundCard->availableToRead() >= BUFFER_READ_LEN) {
			soundCard->read(readBuffer, BUFFER_READ_LEN);

			for (int i = 0; i < BUFFER_READ_LEN; i++) {
				float audio = audioFilter.proc(readBuffer[i]) * 6.0f;
				upsamplingData[i * relation] = audio;
			}

			for (int i = 0; i < upsamplingDataLen; i++) {
				if (i % relation != 0) {
					float dither = ((float)rand() / (float)(RAND_MAX)) / 500000.0f;
					upsamplingData[i] = dither;
				}
				upsamplingData[i] = resamplingFilter.proc(upsamplingData[i]) * 5.0f;
			}

			soundInputBuffer->write(upsamplingData, upsamplingDataLen);

		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		/*int available = soundWriterCircleBuffer->available();
		secondsInBuffer = (float)available / config->outputSamplerate;

		//Если начинается задержка в звуке, то сбросить буффер. Это ликвидирует задержку в звуке.
		if (secondsInBuffer > 0.15) {
			soundWriterCircleBuffer->reset();
			continue;
		}*/

		/*if (available >= len) {
			soundWriterCircleBuffer->read(data, len);
			soundCard->write(data, len);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}*/
	}
	delete[] readBuffer;
	delete[] upsamplingData;
}