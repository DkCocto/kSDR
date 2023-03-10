#include "Hackrf.h"
#include "FirFilter.h"
#include "Filter.h"

hackrf_device* device = NULL;

bool Hackrf::isNeedToSetupFreq() {
	return savedFreq != (viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
}

bool Hackrf::isNeedToSetupLnaGain() {
	return savedLnaGain != viewModel->hackRFModel.lnaGain * 8;
}

bool Hackrf::isNeedToSetupVgnGain() {
	return savedVgaGain != viewModel->hackRFModel.vgaGain * 2;
}

bool Hackrf::isNeedToSetupAmp() {
	return savedAmp != viewModel->hackRFModel.enableAmp;
}

bool Hackrf::isNeedToSetupFilter() {
	return savedBaseband != config->hackrf.basebandFilter;
}

Hackrf::Hackrf(Config* config, CircleBuffer* cb) {
	this->config = config;
	this->cb = cb;
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
		//hackrf->count++;
		//if (hackrf->count == i) {
			//hackrf->count = -1;
			transfer->buffer[2 * i] ^= (uint8_t)0x80;
			transfer->buffer[2 * i + 1] ^= (uint8_t)0x80;

			float I = (((float)transfer->buffer[2 * i] / 130.0f) - 1.0f);
			float Q = (((float)transfer->buffer[2 * i + 1] / 130.0f) - 1.0f);

			//printf("%f\r\n", I);
			//printf("%f\r\n", Q);

			hackrf->cb->write(I);
			hackrf->cb->write(Q);
		//}
		/*if (i % config->inputSamplerateDivider == 0) {
			transfer->buffer[2 * i] ^= (uint8_t)0x80;			
			transfer->buffer[2 * i + 1] ^= (uint8_t)0x80;

			float I = ((float)transfer->buffer[2 * i]) * 1.0f / 128.0f;
			float Q = ((float)transfer->buffer[2 * i + 1]) * 1.0f / 128.0f;

			hackrf->cb->write(I);
			hackrf->cb->write(Q);
		}*/
	}

	return 0;
}


bool Hackrf::init() {
	viewModel = Display::instance->viewModel;

	uint8_t amp = config->hackrf.rxAmp;
	uint8_t antenna = 0;
	uint32_t baseband = config->hackrf.basebandFilter;

	//0, 8, 16, 24, 32, 40 
	uint32_t lna_gain = config->hackrf.lnaGain;

	//0 .. 62 step 2
	uint32_t vga_gain = config->hackrf.vgaGain;

	hackrf_error result;

	result = (hackrf_error)hackrf_init();
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_init() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	const char* serial_number = NULL;
	//result = (hackrf_error)hackrf_open_by_serial(serial_number, &device);
	result = (hackrf_error)hackrf_open(&device);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_open() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_hw_sync_mode(device, (uint8_t)0);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_hw_sync_mode() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_sample_rate(device, config->hackrf.deviceSamplingRate);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_sample_rate() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}
	
	int64_t freq_hz = config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);

	result = (hackrf_error)hackrf_set_freq(device, freq_hz);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_freq() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_amp_enable() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_antenna_enable(device, antenna);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_antenna_enable() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_vga_gain(device, vga_gain);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_vga_gain() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}
	result = (hackrf_error)hackrf_set_lna_gain(device, lna_gain);
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_lna_gain() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
	if (result != HACKRF_SUCCESS) {
		status->err.append("hackrf_set_baseband_filter_bandwidth() failed: ");
		status->err.append(hackrf_error_name(result));
		status->isInitProcessOccured = true;
		status->isOK = false;
		return status->isOK;
	}

	result = startRX();
	if (result != HACKRF_SUCCESS) {
		status->isOK = false;
		status->isInitProcessOccured = true;
		status->err.append("hackrf_start_rx() failed: ");
		status->err.append(hackrf_error_name(result));
		status->err.append("\n Error code: ");
		status->err.append(std::to_string(result));
	} else {
		status->isInitProcessOccured = true;
		status->isOK = true;
	}

	return status->isOK;
}

void Hackrf::stopRX() {
	hackrf_stop_rx(device);
}

hackrf_error Hackrf::startRX() {
	return (hackrf_error)hackrf_start_rx(device, rx_callback, config);
}


void Hackrf::setFreq(uint64_t freq) {

	//lo = 85Mhz...4200Mhz
	//if = 2300Mhz...2700Mhz
	//																if			lo
	//auto result = (hackrf_error)hackrf_set_freq_explicit(device, static_cast<uint64_t>(viewModel->test) * 1000 - freq, static_cast<uint64_t>(viewModel->test) * 1000, RF_PATH_FILTER_LOW_PASS);

	hackrf_error result = (hackrf_error)hackrf_set_freq(device, freq);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_freq() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Frequency set: %d\r\n", (int)freq);
	savedFreq = freq;
}

void Hackrf::setLnaGain(uint32_t gain) {
	hackrf_error result = (hackrf_error)hackrf_set_lna_gain(device, gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_lna_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Gain LNA set: %d\r\n", (int)gain);
	savedLnaGain = gain;
}

void Hackrf::setVgaGain(uint32_t gain) {
	hackrf_error result = (hackrf_error)hackrf_set_vga_gain(device, gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_vga_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Gain VGA set: %d\r\n", (int)gain);
	savedVgaGain = gain;
}

void Hackrf::setBaseband(int baseband) {
	hackrf_error result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_baseband_filter_bandwidth() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	savedBaseband = baseband;
	printf("Baseband set: %d\r\n", (int)baseband);
}

void Hackrf::setConfiguration() {
	if (isNeedToSetupFreq()) setFreq((uint64_t)(viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0)));
	if (isNeedToSetupLnaGain()) setLnaGain((uint32_t)(viewModel->hackRFModel.lnaGain * 8));
	if (isNeedToSetupVgnGain()) setVgaGain((uint32_t)(viewModel->hackRFModel.vgaGain * 2));
	if (isNeedToSetupAmp()) enableAmp((uint8_t)viewModel->hackRFModel.enableAmp);
	if (isNeedToSetupFilter()) setBaseband(config->hackrf.basebandFilter);
}

void Hackrf::enableAmp(uint8_t amp) {
	hackrf_error result = (hackrf_error)hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_amp_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	savedAmp = amp;
	printf("AMP enabled set: %d\r\n", (int)amp);
}