#include "RTLDevice.h"

int RTLDevice::nearest_gain(int targetGain) {
	int i, r, err1, err2, count, nearest;
	int* gains;
	r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		fprintf(stderr, "WARNING: Failed to enable manual gain.\n");
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

std::vector<uint32_t>* RTLDevice::listDevices() {
	int i, device_count, device, offset;
	char* s2;
	char vendor[256], product[256], serial[256];
	device_count = rtlsdr_get_device_count();
	if (!device_count) {
		fprintf(stderr, "No supported devices found.\n");
		return NULL;
	}

	std::vector<uint32_t>* list = new std::vector<uint32_t>;

	fprintf(stderr, "Found %d device(s):\n", device_count);
	for (i = 0; i < device_count; i++) {
		rtlsdr_get_device_usb_strings(i, vendor, product, serial);
		fprintf(stderr, "  %d:  %s, %s, SN: %s\n", i, vendor, product, serial);
		list->push_back(i);
	}
	fprintf(stderr, "\n");

	return list;
}

int RTLDevice::openDevice(uint32_t deviceIndex) {
	int r = rtlsdr_open(&device, deviceIndex);
	if (r < 0) {
		fprintf(stderr, "Failed to open rtlsdr device #%d.\n", deviceIndex);
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
		fprintf(stderr, "WARNING: Failed to set sample rate.\n");
	} else {
		fprintf(stderr, "Sampling at %u S/s.\n", sampRate);
	}
	return r;
}

int RTLDevice::setFrequency(uint32_t frequency) {
	int r;
	r = rtlsdr_set_center_freq(device, frequency);
	if (r < 0) {
		fprintf(stderr, "WARNING: Failed to set center freq.\n");
	} else {
		fprintf(stderr, "Tuned to %u Hz.\n", frequency);
	}
	return r;
}

int RTLDevice::enableAutoGain() {
	int r;
	r = rtlsdr_set_tuner_gain_mode(device, 0);
	if (r != 0) {
		fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
	}
	else {
		fprintf(stderr, "Tuner gain set to automatic.\n");
	}
	return r;
}

int RTLDevice::setGain(int gain) {
	int r;
	r = rtlsdr_set_tuner_gain_mode(device, 1);
	if (r < 0) {
		fprintf(stderr, "WARNING: Failed to enable manual gain.\n");
		return r;
	}
	r = rtlsdr_set_tuner_gain(device, gain);
	if (r != 0) {
		fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
	}
	else {
		fprintf(stderr, "Tuner gain set to %0.2f dB.\n", gain / 10.0);
	}
	return r;
}

int RTLDevice::resetBuffer() {
	int r;
	r = rtlsdr_reset_buffer(device);
	if (r < 0) {
		fprintf(stderr, "WARNING: Failed to reset buffers.\n");
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

	((CircleBuffer*)ctx)->write(buf, len);

	//if (ctx) {
		/*if (do_exit)
			return;

		if ((bytes_to_read > 0) && (bytes_to_read < len)) {
			len = bytes_to_read;
			do_exit = 1;
			rtlsdr_cancel_async(dev);
		}

		if (fwrite(buf, 1, len, (FILE*)ctx) != len) {
			fprintf(stderr, "Short write, samples lost, exiting!\n");
			rtlsdr_cancel_async(dev);
		}

		if (bytes_to_read > 0)
			bytes_to_read -= len;*/
			//}
}

int RTLDevice::startAsync(CircleBuffer* circleBuffer) {
	return rtlsdr_read_async(device, RTLDevice::rtlsdr_callback, (void*)circleBuffer, 0, RTL_READ_BUFFER);
}
