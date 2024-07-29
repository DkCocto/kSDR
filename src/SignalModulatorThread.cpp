#include "SignalModulatorThread.h"

SignalModulatorThread::SignalModulatorThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* soundCard) {
	this->config = config;
	this->outputSignalBuffer = soundInputBuffer;
	this->soundCard = soundCard;

	so = SinOscillator(TONE_SIGNAL_FREQ, config->inputSamplerateSound);

	BUFFER_READ_LEN = config->audioReadFrameLen; //2

	ssbModulation = new SSBModulation(config, BUFFER_READ_LEN);
}

SignalModulatorThread::~SignalModulatorThread() {
	delete ssbModulation;
}

std::thread SignalModulatorThread::start() {
	std::thread p(&SignalModulatorThread::run, this);
	return p;
}

void SignalModulatorThread::run() {

	isWorking_ = true;

	float* readBuffer = new float[BUFFER_READ_LEN]; // count = 2

	while (true) {
		if (!config->WORKING) {
			printf("SoundCardInputReaderThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (currentStatus == START_READING) {
			soundCard->startInput();
			currentStatus = READING;
		}

		if (currentStatus == PAUSE) {
			soundCard->stopInput();
			currentStatus = REST;
			continue;
		}

		if (currentStatus == REST) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}

		if (soundCard->availableToRead() >= BUFFER_READ_LEN) {
			soundCard->read(readBuffer, BUFFER_READ_LEN);

			if (config->transmit.sendToneSignal) {
				for (int i = 0; i < BUFFER_READ_LEN; i++) {
					readBuffer[i] = so.nextSample();
				}
			}
			//config->receiver.modulation == USB

			auto ssbData = ssbModulation->processData(readBuffer);
			outputSignalBuffer->write(ssbData->data, ssbData->len);

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	delete[] readBuffer;
}