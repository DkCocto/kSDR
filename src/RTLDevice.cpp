#include "RTLDevice.h"

int RTLDevice::nearest_gain(int targetGain) {
	int i, r, err1, err2, count, nearest;
	int* gains;
	r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to enable manual gain.\n");
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
	delete gains;
	return nearest;
}

bool RTLDevice::isNeedToSetFreq() {
	return savedFreq != (viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
}

bool RTLDevice::isNeedToSetGain() {
	return savedGain != config->rtl.gain;
}

RTLDevice::RTLDevice(Config* config, CircleBuffer* cb) {
	this->config = config;
	this->cb = cb;
}

RTLDevice::~RTLDevice() {
	if (status->isOK) {
		if (rtlsdr_cancel_async(device) == 0) closeDevice();
	}
}

bool RTLDevice::init() {
	viewModel = Display::instance->getViewModel();

	vector<uint32_t>* list = listDevices();
	if (list != NULL) {
		openDevice(list->at(0));
	} else {
		status->isOK = false;
		status->err = "Can't find rtl device! Check usb connection or drivers.";
		status->isInitProcessOccured = true;
		return status->isOK;
	}
	setFrequency(config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
	setSampleRate(config->rtl.deviceSamplingRate / config->inputSamplerateDivider);

	enableAutoGain();

	resetBuffer();

	std::thread t1([&] { 
		startAsync(); 
	});
	t1.detach();

	status->isOK = true;
	status->isInitProcessOccured = true;
	return status->isOK;
}

std::vector<uint32_t>* RTLDevice::listDevices() {
	int i, device_count, device, offset;
	char* s2;
	char vendor[256], product[256], serial[256];
	device_count = rtlsdr_get_device_count();
	if (!device_count) {
		if (DEBUG) fprintf(stderr, "No supported devices found.\n");
		return NULL;
	}

	std::vector<uint32_t>* list = new std::vector<uint32_t>;

	if (DEBUG) fprintf(stderr, "Found %d device(s):\n", device_count);
	for (i = 0; i < device_count; i++) {
		rtlsdr_get_device_usb_strings(i, vendor, product, serial);
		if (DEBUG) fprintf(stderr, "  %d:  %s, %s, SN: %s\n", i, vendor, product, serial);
		list->push_back(i);
	}
	if (DEBUG) fprintf(stderr, "\n");

	return list;
}

int RTLDevice::openDevice(uint32_t deviceIndex) {
	int r = rtlsdr_open(&device, deviceIndex);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "Failed to open rtlsdr device #%d.\n", deviceIndex);
		return -1;
	}
	return 1;
}

rtlsdr_dev_t* RTLDevice::getDevice() {
	return device;
}

int RTLDevice::setSampleRate(uint32_t sampRate) {
	int r;
	r = rtlsdr_set_sample_rate(device, sampRate);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to set sample rate.\n");
	} else {
		if (DEBUG) fprintf(stderr, "Sampling at %u S/s.\n", sampRate);
	}
	return r;
}

int RTLDevice::setFrequency(uint32_t frequency) {
	int r;
	r = rtlsdr_set_center_freq(device, frequency);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to set center freq.\n");
	} else {
		if (DEBUG) fprintf(stderr, "Tuned to %u Hz.\n", frequency);
		savedFreq = frequency;
	}
	return r;
}

int RTLDevice::enableAutoGain() {
	int r;
	r = rtlsdr_set_tuner_gain_mode(device, 0);
	if (r != 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
	}
	else {
		if (DEBUG) fprintf(stderr, "Tuner gain set to automatic.\n");
	}
	return r;
}

int RTLDevice::setGain(int gain) {
	int r;
	r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to enable manual gain.\n");
		return r;
	}
	r = rtlsdr_set_tuner_gain(device, gain);
	if (r != 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
	}
	else {
		if (DEBUG) fprintf(stderr, "Tuner gain set to %0.2f dB.\n", gain / 10.0);
		savedGain = gain;
	}
	return r;
}

int RTLDevice::resetBuffer() {
	int r;
	r = rtlsdr_reset_buffer(device);
	if (r < 0) {
		if (DEBUG) fprintf(stderr, "WARNING: Failed to reset buffers.\n");
	}
	return r;
}

int RTLDevice::readSync(uint8_t* buffer, uint32_t blockSize, int* n_read) {
	int r = rtlsdr_read_sync(device, buffer, blockSize, n_read);
	return r;
}

int RTLDevice::closeDevice() {
	return rtlsdr_close(device);
}

void RTLDevice::rtlsdr_callback(unsigned char* buf, uint32_t len, void* ctx) {
	RTLDevice* rtlDevice = (RTLDevice*)ctx;
	for (int i = 0; i < len; i++) {
		rtlDevice->cb->write(((float)buf[i] - 127.5) / 128.0);
	}
}

int RTLDevice::startAsync() {
	return rtlsdr_read_async(device, RTLDevice::rtlsdr_callback, (void*)this, 0, 512);
}

void RTLDevice::setConfiguration() {
	if (isNeedToSetFreq()) setFrequency(viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
	if (isNeedToSetGain()) setGain(config->rtl.gain);
}
