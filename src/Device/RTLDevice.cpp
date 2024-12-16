#include "RTLDevice.h"

void RTLDevice::stop() {
	if (initResult.status == INIT_OK) {
		processing = false;
		//while (rtlsdr_cancel_async(device) != 0);
		//while (deviceWorking);
		//resetBuffer();
		closeDevice();
		//delete device;
	}
}

int RTLDevice::closeDevice() {
	return rtlsdr_close(device);
}

RTLDevice::~RTLDevice() {
	printf("~RTLDevice()\r\n");
	stop();
	delete bufferForSpec;
	delete bufferForProc;
}

int RTLDevice::startAsync() {
	return rtlsdr_read_async(device, RTLDevice::rtlsdr_callback, (void*)this, 0, 512);
}

Result RTLDevice::start() {
	initResult = { CREATED_BUT_NOT_INIT, std::string("") };

	std::vector<uint32_t>* list = listDevices();
	if (list != NULL) {
		openDevice(list->at(0));
	} else {
		initResult.status = INIT_FAULT;
		initResult.err.append("Can't find rtl device! Check usb connection or drivers.");
		return initResult;
	}

	int shift = (config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0;
	int r = setFreq((uint32_t)(config->startFrequency + shift));

	if (r < 0) {
		initResult.status = INIT_FAULT;
		return initResult;
	}

	if (setSampleRate((uint32_t)(config->rtl.deviceSamplingRate / config->inputSamplerateDivider)) < 0) {
		initResult.status = INIT_FAULT;
		return initResult;
	}

	if (enableAutoGain(false) < 0) {
		initResult.status = INIT_FAULT;
		return initResult;
	}

	if (setGain(config->rtl.gain) < 0) {
		initResult.status = INIT_FAULT;
		return initResult;
	}

	if (resetBuffer() < 0) {
		initResult.status = INIT_FAULT;
		return initResult;
	}

	//!!!ENABLE DIRECT SAMPLING!!!
	rtlsdr_set_direct_sampling(device, 1);

	std::thread t1([&] {
		processing = true;
		unsigned char buf[16384];
		while (processing) {
			int n_read = 0;
			rtlsdr_read_sync(device, &buf, 16384, &n_read);
			getBufferForProc()->write(buf, n_read);
			getBufferForSpec()->write(buf, n_read);

		}
		//delete[] buf;
		//startAsync(); 
		//deviceWorking = false;
		//resetBuffer();
	});
	t1.detach();

	initResult.status = INIT_OK;
	return initResult;
}

CircleBufferNew<unsigned char>* RTLDevice::getBufferForSpec() {
	return bufferForSpec;
}

CircleBufferNew<unsigned char>* RTLDevice::getBufferForProc() {
	return bufferForProc;
}

float RTLDevice::prepareData(unsigned char val) {
	return ((float)val - 127.5) / 128.0;
}

std::vector<uint32_t>* RTLDevice::listDevices() {
	int i, device_count;
	char vendor[256], product[256], serial[256];
	device_count = rtlsdr_get_device_count();
	if (!device_count) {
		if (DEBUG) printf("No supported devices found.\n");
		return NULL;
	}

	std::vector<uint32_t>* list = new std::vector<uint32_t>;

	if (DEBUG) printf("Found %d device(s):\n", device_count);
	for (i = 0; i < device_count; i++) {
		rtlsdr_get_device_usb_strings(i, vendor, product, serial);
		if (DEBUG) printf("  %d:  %s, %s, SN: %s\n", i, vendor, product, serial);
		list->push_back(i);
	}
	if (DEBUG) printf("\n");

	return list;
}

int RTLDevice::openDevice(uint32_t deviceIndex) {
	int r = rtlsdr_open(&device, deviceIndex);
	if (r < 0) {
		if (DEBUG) printf("Failed to open rtlsdr device #%d.\n", deviceIndex);
		return -1;
	}
	return 1;
}

int RTLDevice::setFreq(uint32_t frequency) {
	int r = rtlsdr_set_center_freq(device, frequency);
	if (r < 0) {
		if (DEBUG) printf("WARNING: Failed to set center freq.\n");
	} else {
		if (DEBUG) printf("Tuned to %d Hz.\n", frequency);
	}
	return r;
}

int RTLDevice::setSampleRate(uint32_t sampRate) {
	int r = rtlsdr_set_sample_rate(device, sampRate);
	if (r < 0) {
		if (DEBUG) printf("WARNING: Failed to set sample rate.\n");
	} else {
		if (DEBUG) printf("Sampling at %u S/s.\n", sampRate);
	}
	return r;
}

int RTLDevice::enableAutoGain(bool enable) {
	int r = rtlsdr_set_tuner_gain_mode(device, enable);
	if (r != 0) {
		if (DEBUG) printf("WARNING: Failed to set tuner gain.\n");
	} else {
		if (DEBUG) printf("Tuner gain set to automatic.\n");
	}
	return r;
}

int RTLDevice::resetBuffer() {
	int r = rtlsdr_reset_buffer(device);
	if (r < 0) {
		if (DEBUG) printf("WARNING: Failed to reset buffers.\n");
	}
	return r;
}

void RTLDevice::rtlsdr_callback(unsigned char* buf, uint32_t len, void* ctx) {
	RTLDevice* rtlDevice = (RTLDevice*)ctx;

	rtlDevice->getBufferForProc()->write(buf, len);
	rtlDevice->getBufferForSpec()->write(buf, len);
}

int RTLDevice::setGain(int gain) {
	/*int r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		if (DEBUG) printf("WARNING: Failed to enable manual gain.\n");
		return r;
	}*/
	int r = rtlsdr_set_tuner_gain(device, nearest_gain(gain));
	if (r != 0) {
		if (DEBUG) printf("WARNING: Failed to set tuner gain.\n");
	} else {
		if (DEBUG) printf("Tuner gain set to %0.2f dB.\n", gain / 10.0);
	}
	return r;
}

int RTLDevice::nearest_gain(int targetGain) {
	int i, r, err1, err2, count, nearest;
	int* gains;
	r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		if (DEBUG) printf("WARNING: Failed to enable manual gain.\n");
		return r;
	}
	count = rtlsdr_get_tuner_gains(device, NULL);
	if (count <= 0) {
		return 0;
	}
	gains = new int[count];
	//malloc(sizeof(int) * count);
	count = rtlsdr_get_tuner_gains(device, gains);
	nearest = gains[0];
	for (i = 0; i < count; i++) {
		err1 = abs(targetGain - nearest);
		err2 = abs(targetGain - gains[i]);
		if (err2 < err1) {
			nearest = gains[i];
		}
	}
	delete[] gains;
	return nearest;
}