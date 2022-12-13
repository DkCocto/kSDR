#include "Hackrf.h"

hackrf_device* device = NULL;

Hackrf::Hackrf(Config* config, CircleBuffer* cb) {
	this->config = config;
	this->cb = cb;

	hackrf_error result = (hackrf_error)hackrf_init();
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_init() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	const char* serial_number = NULL;
	result = (hackrf_error)hackrf_open_by_serial(serial_number, &device);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_open() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
}



Hackrf::~Hackrf() {
	hackrf_error result;
	if (device != NULL) {
		close();
		hackrf_exit();
	}
}

void Hackrf::close() {
	hackrf_error result = (hackrf_error)hackrf_stop_rx(device);
	if (result != HACKRF_SUCCESS) {

	}
	result = (hackrf_error)hackrf_close(device);
}

int Hackrf::rx_callback(hackrf_transfer* transfer) {
	int bytes_to_write = transfer->valid_length;

	Hackrf* hackrf = (Hackrf*)transfer->rx_ctx;

	hackrf->cb->write(transfer->buffer, bytes_to_write);

	return 0;
}

/*bool Hackrf::isNeedToSetFreq() {
	return savedFreq != viewModel->centerFrequency;
}

bool Hackrf::isNeedToLnaGain() {
	return savedLnaGain != viewModel->lnaGain;
}*/


void Hackrf::init() {

	uint8_t amp = 0;
	uint8_t antenna = 0;
	uint32_t baseband = 1750000;

	//0, 8, 16, 24, 32, 40 
	unsigned int lna_gain = 32;

	//0 .. 62 step 2
	unsigned int vga_gain = 28;

	hackrf_error result;

	//hackrf_set_hw_sync_mode(device, (uint8_t)0);

	result = (hackrf_error) hackrf_set_sample_rate(device, config->inputSamplerate);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_sample_rate() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	int64_t freq_hz = config->startFrequency;

	result = (hackrf_error) hackrf_set_freq(device, freq_hz);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_freq() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error) hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_amp_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error) hackrf_set_antenna_enable(device, antenna);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_antenna_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	//printf("%d \r\n", hackrf_compute_baseband_filter_bw_round_down_lt(5000000));

	result = (hackrf_error) hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw_round_down_lt(baseband));
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_baseband_filter_bandwidth() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error) hackrf_set_vga_gain(device, vga_gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_vga_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	result = (hackrf_error) hackrf_set_lna_gain(device, lna_gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_lna_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	result = startRX();
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_start_rx() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
}

void Hackrf::stopRX() {
	hackrf_stop_rx(device);
}

hackrf_error Hackrf::startRX() {
	return (hackrf_error)hackrf_start_rx(device, rx_callback, this);
}


void Hackrf::setFreq(uint64_t freq) {
	if (savedFreq != freq) {
		hackrf_error result = (hackrf_error)hackrf_set_freq(device, (uint64_t)freq);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_freq() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedFreq = freq;
	}	
}

void Hackrf::setLnaGain(unsigned int gain) {
	if (savedLnaGain != gain) {
		hackrf_error result = (hackrf_error)hackrf_set_lna_gain(device, gain);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_lna_gain() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedLnaGain = gain;
	}
	
}

void Hackrf::setVgaGain(unsigned int gain) {
	if (savedVgaGain != gain) {
		hackrf_error result = (hackrf_error)hackrf_set_vga_gain(device, gain);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_vga_gain() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedVgaGain = gain;
	}
	
}

void Hackrf::enableAmp(bool enabled) {
	if (savedAmp != enabled) {
		hackrf_error result = (hackrf_error)hackrf_set_amp_enable(device, (enabled == true) ? 1 : 0);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_amp_enable() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedAmp = enabled;
	}
}