#include "Hackrf.h"
#include "FirFilter.h"
#include "Filter.h"

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
	//result = (hackrf_error)hackrf_open_by_serial(serial_number, &device);
	result = (hackrf_error)hackrf_open(&device);
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
	int bytes_to_write = transfer->buffer_length;

	Config* config = (Config*)transfer->rx_ctx;
	Hackrf* hackrf = (Hackrf*)config->device;

	for (int i = 0; i < (bytes_to_write / 2 - 1); i++) { 

		if (i % config->inputSamplerateDivider == 0) {
			transfer->buffer[2 * i] ^= (uint8_t)0x80;			
			transfer->buffer[2 * i + 1] ^= (uint8_t)0x80;

			float I = (((float)transfer->buffer[2 * i] / 130.0f) - 1.0f);
			float Q = (((float)transfer->buffer[2 * i + 1] / 130.0f) - 1.0f);

			//printf("%f\r\n", I);
			//printf("%f\r\n", Q);

			hackrf->cb->write(I);
			hackrf->cb->write(Q);
		}
	}

	return 0;
}

/*bool Hackrf::isNeedToSetFreq() {
	return savedFreq != viewModel->centerFrequency;
}

bool Hackrf::isNeedToLnaGain() {
	return savedLnaGain != viewModel->lnaGain;
}*/


void Hackrf::init() {

	uint8_t amp = config->hackrf.rxAmp;
	uint8_t antenna = 0;
	uint32_t baseband = config->hackrf.basebandFilter;

	//0, 8, 16, 24, 32, 40 
	uint32_t lna_gain = config->hackrf.lnaGain;

	//0 .. 62 step 2
	uint32_t vga_gain = config->hackrf.vgaGain;

	hackrf_error result;

	hackrf_set_hw_sync_mode(device, (uint8_t)0);
	//hackrf_set_sample_rate_manual
	//hackrf_set_sample_rate
	result = (hackrf_error)hackrf_set_sample_rate(device, config->hackrf.deviceSamplingRate);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_sample_rate() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	
	int64_t freq_hz = config->startFrequency;

	//result = (hackrf_error)hackrf_set_freq(device, freq_hz);
	//result = (hackrf_error)hackrf_set_freq_explicit(device, 2408100000, 2401000000, RF_PATH_FILTER_LOW_PASS);

	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_freq() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}



	result = (hackrf_error)hackrf_set_freq(device, freq_hz);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_freq() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error)hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_amp_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error)hackrf_set_antenna_enable(device, antenna);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_antenna_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	//printf("%d \r\n", hackrf_compute_baseband_filter_bw(baseband));

	result = (hackrf_error)hackrf_set_vga_gain(device, vga_gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_vga_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	result = (hackrf_error)hackrf_set_lna_gain(device, lna_gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_lna_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_baseband_filter_bandwidth() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}

	hackrf_set_txvga_gain(device, 0);

	/* range 0-47 step 1db */
	//hackrf_set_txvga_gain(device, 0);

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
	return (hackrf_error)hackrf_start_rx(device, rx_callback, config);
}


void Hackrf::setFreq(uint64_t freq) {
	if (savedFreq != freq) {
		//hackrf_error result = (hackrf_error)hackrf_set_freq_explicit(device, 2400000000 - freq, 2400000000, RF_PATH_FILTER_LOW_PASS);
		hackrf_error result = (hackrf_error)hackrf_set_freq(device, freq);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_freq() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedFreq = freq;
	}
}

void Hackrf::setLnaGain(uint32_t gain) {
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

void Hackrf::setVgaGain(uint32_t gain) {
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

void Hackrf::setBaseband(uint32_t baseband) {
	if (savedBaseband != baseband) {
		hackrf_error result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_baseband_filter_bandwidth() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		printf("setBaseband\r\n");
		savedBaseband = baseband;
	}
}

int Hackrf::parse_u32(char* s, uint32_t* const value)
{
	uint_fast8_t base = 10;
	char* s_end;
	uint64_t ulong_value;

	if (strlen(s) > 2) {
		if (s[0] == '0') {
			if ((s[1] == 'x') || (s[1] == 'X')) {
				base = 16;
				s += 2;
			}
			else if ((s[1] == 'b') || (s[1] == 'B')) {
				base = 2;
				s += 2;
			}
		}
	}

	s_end = s;
	ulong_value = strtoul(s, &s_end, base);
	if ((s != s_end) && (*s_end == 0)) {
		*value = (uint32_t)ulong_value;
		return HACKRF_SUCCESS;
	}
	else {
		return HACKRF_ERROR_INVALID_PARAM;
	}
}

void Hackrf::enableAmp(uint8_t amp) {
	if (savedAmp != amp) {
		hackrf_error result = (hackrf_error)hackrf_set_amp_enable(device, amp);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"hackrf_set_amp_enable() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}
		savedAmp = amp;
	}
}