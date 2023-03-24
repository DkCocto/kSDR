#include "RTLDeviceReaderThread.h"

RTLDeviceReaderThread::RTLDeviceReaderThread(CircleBuffer* cb) {
	this->cb = cb;
}

RTLDeviceReaderThread::~RTLDeviceReaderThread() {
	rtl.resetBuffer();
	rtl.closeDevice();
}

void RTLDeviceReaderThread::process() {
	initRTLDevice();
	int read = 0;
	uint8_t* buf = new uint8_t[RTL_READ_BUFFER];
	rtl.resetBuffer();

	if (rtl.startAsync(cb) != 0) printf("error\r\n");
	while (true) {
		if (savedFreq != Display::instance->viewModel.frequency) {
			savedFreq = Display::instance->viewModel.frequency;
			rtl.resetBuffer();
			rtl.setFrequency((uint32_t)savedFreq);
		}

		if (savedGain != Display::instance->viewModel.gain) {
			savedGain = Display::instance->viewModel.gain;
			rtl.resetBuffer();
			rtl.setGain(savedGain);
		}

		rtl.readSync(buf, RTL_READ_BUFFER, &read);

		cb->write(buf, read);
	}
}

std::thread RTLDeviceReaderThread::start() {
	std::thread p(&RTLDeviceReaderThread::process, this);
	return p;
}

void RTLDeviceReaderThread::initRTLDevice() {
	std::vector<uint32_t>* list = rtl.listDevices();
	if (list != NULL) {
		rtl.openDevice(list->at(0));
	}
	rtl.setFrequency(savedFreq);
	rtl.setSampleRate(INPUT_SAMPLE_RATE);
	rtl.enableAutoGain();
}