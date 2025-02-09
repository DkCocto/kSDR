#pragma once

#include "DeviceN.h"
#include "../CircleBufferNew.h"
#include "portaudio/portaudio.h"

class SoundCardDevice : public DeviceN {

private:

	CircleBufferNew<float>* bufferForSpec;
	CircleBufferNew<float>* bufferForProc;

	PaStream* inputStream = NULL;
	PaError err;

	bool stop_ = false;

	int buffer_read_len;

public:

	float* readBuffer; // count = 2

	SoundCardDevice(Config* config) : DeviceN(config) {
		bufferForSpec = new CircleBufferNew<float>(config);
		bufferForProc = new CircleBufferNew<float>(config);
		buffer_read_len = config->soundcardDevice.inputFrameCount;
		readBuffer = new float[buffer_read_len];
	};

	~SoundCardDevice() {
		delete bufferForSpec;
		delete bufferForProc;
		delete[] readBuffer;
	};

	void run() {
		while (!stop_) {
			if (inputStream != nullptr) {
				if (Pa_GetStreamReadAvailable(inputStream)) {
					err = Pa_ReadStream(inputStream, readBuffer, buffer_read_len / 2);
					bufferForSpec->write(readBuffer, buffer_read_len);
					bufferForProc->write(readBuffer, buffer_read_len);
				}
				else {
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}
			}
			else {
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}
	}


	Result start() {
		stop_ = false;
		Result initResult = { INIT_OK, std::string("") };
		std::thread p(&SoundCardDevice::run, this);
		p.detach();
		return initResult;
	}

	void stop() {
		stop_ = true;
	}

	CircleBufferNew<float>* getBufferForSpec() {
		return bufferForSpec;
	}
	CircleBufferNew<float>* getBufferForProc() {
		return bufferForProc;
	}

	float prepareData(float val) {
		return val;
	}

	void setInputStream(PaStream* input) {
		inputStream = input;
	}

};