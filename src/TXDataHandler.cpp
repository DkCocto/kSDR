#include "TXDataHandler.h"

TXDataHandler::TXDataHandler(Config* config, CircleBufferNew<float>* inputBuffer, CircleBufferNew<float>* outputBuffer) {
	this->config = config;
	this->inputBuffer = inputBuffer;
	this->outputBuffer = outputBuffer;

}

TXDataHandler::~TXDataHandler() { }

void TXDataHandler::run() {
	isWorking_ = true;

	ssb.setConfig(config);
	ssb.setFreq(5000);

	float* data = new float[ssb.getInputBufferLen()];

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

		if (inputBuffer->available() >= ssb.getInputBufferLen()) {

			/*inputBuffer->read(data, ssb.getInputBufferLen());
			float* processedData = ssb.processData(data);
			outputBuffer->write(processedData, ssb.getOutputBufferLen());*/

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}

	delete[] data;

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