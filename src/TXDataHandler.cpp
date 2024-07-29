#include "TXDataHandler.h"

TXDataHandler::TXDataHandler(Config* config, CircleBufferNew<float>* inputBuffer, CircleBufferNew<float>* outputBuffer) {
	this->config = config;
	this->inputBuffer = inputBuffer;
	this->outputBuffer = outputBuffer;
}

TXDataHandler::~TXDataHandler() { }

void TXDataHandler::run() {
	isWorking_ = true;

	while (true) {
		if (!config->WORKING) {
			printf("TXDataHandler Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (!processing) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

}

std::thread TXDataHandler::start() {
	std::thread p(&TXDataHandler::run, this);
	return p;
}

void TXDataHandler::pause() {
	processing = false;
}

void TXDataHandler::continueProcess() {
	processing = true;
}