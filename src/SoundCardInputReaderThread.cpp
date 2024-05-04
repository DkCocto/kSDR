#include "SoundCardInputReaderThread.h"
#include "Filter.h"
#include "FirFilter.h"
#include "CosOscillator.h"

//CosOscillator* cosOscil;

SoundCardInputReaderThread::SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* soundCard) {
	this->config = config;
	this->soundInputBuffer = soundInputBuffer;
	this->soundCard = soundCard;
	firFilter.init(firFilter.LOWPASS, firFilter.BARTLETT, 128, 3200, 0, config->inputSamplerateSound);
	firFilter2.init(firFilter.LOWPASS, firFilter.BARTLETT, 200, 8000, 0, config->currentWorkingInputSamplerate);
	//cosOscil = new CosOscillator(500000, config->currentWorkingInputSamplerate);
}

SoundCardInputReaderThread::~SoundCardInputReaderThread() {

}

std::thread SoundCardInputReaderThread::start() {
	std::thread p(&SoundCardInputReaderThread::run, this);
	return p;
}

void SoundCardInputReaderThread::run() {
	//float secondsInBuffer = 0.0;

	isWorking_ = true;

	float* readBuffer = new float[config->audioReadFrameLen];

	long count = 0;
	int relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound; // 4 000 000 / 20 000 = 200
	int upsamplingDataLen = config->audioReadFrameLen * relation; // 8 * 200 = 1600

	float* upsamplingData = new float[upsamplingDataLen];
	memset(upsamplingData, 0, sizeof(float) * upsamplingDataLen);

	while (true) {
		if (!config->WORKING) {
			printf("SoundCardInputReaderThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (soundCard->availableToRead() >= config->audioReadFrameLen) {
			soundCard->read(readBuffer, config->audioReadFrameLen);

			//Фильтруем звук и сужаем полосу до 3кгц
			for (int i = 0; i < config->audioReadFrameLen; i++) {
				readBuffer[i] = firFilter.proc(readBuffer[i]);
			}

			//делаем ресемплинг до основной частоты дескретизации
			for (int i = 0; i < config->audioReadFrameLen; i++) {
				//i = 0 -> 0
				//i = 1 -> 200
				//i = 7 -> 1400
				upsamplingData[i * relation] = readBuffer[i];
			}


			//фильтруем получившейся сигнал
			for (int i = 0; i < upsamplingDataLen; i++) {
				if (i % relation != 0) {
					float dither = ((float)rand() / (float)(RAND_MAX)) / 100000.0f;
					upsamplingData[i] = dither;
				}
				upsamplingData[i] = firFilter2.proc(upsamplingData[i]);
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

	delete[] upsamplingData;
	delete[] readBuffer;

}