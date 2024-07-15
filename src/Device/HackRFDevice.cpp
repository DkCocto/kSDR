#include "HackRFDevice.h"

int HackRFDevice::rx_callback(hackrf_transfer* transfer) {
	HackRFDevice* hackRFDevice = (HackRFDevice*)transfer->rx_ctx;

	hackRFDevice->getBufferForSpec()->write(transfer->buffer, transfer->buffer_length);
	hackRFDevice->getBufferForProc()->write(transfer->buffer, transfer->buffer_length);

	return 0;
}

int HackRFDevice::tx_callback(hackrf_transfer* transfer) {
	HackRFDevice* hackRFDevice = (HackRFDevice*)transfer->rx_ctx;

	TransmittingData* transmittingData = hackRFDevice->transmittingData;

	if (transmittingData != nullptr) {
		Signal* signal = transmittingData->nextBuffer();

		if (signal == nullptr) return 0;

		for (int i = 0; i < HACKRF_TX_BUFFER_HALF_LEN; i++) {
			transfer->buffer[2 * i] = hackRFDevice->chuchka((uint8_t)(((signal[i].I + 1.0f) / 2.0f) * 255.0f));
			transfer->buffer[2 * i + 1] = hackRFDevice->chuchka((uint8_t)(((signal[i].Q + 1.0f) / 2.0f) * 255.0f));
		}

		hackRFDevice->getBufferForSpec()->write(transfer->buffer, transfer->buffer_length);
	}

	return 0;
}

uint8_t HackRFDevice::chuchka(uint8_t val) {
	if (val >= 128 && val <= 255) return val - 128;
	if (val >= 0 && val <= 127) return val + 128;
}

void HackRFDevice::setFreq(uint64_t frequency) {
	//lo = 85Mhz...4200Mhz
	//if = 2300Mhz...2700Mhz
	//																if			lo
	//auto result = (hackrf_error)hackrf_set_freq_explicit(device, static_cast<uint64_t>(viewModel->test) * 1000 - freq, static_cast<uint64_t>(viewModel->test) * 1000, RF_PATH_FILTER_LOW_PASS);

	hackrf_error result = (hackrf_error)hackrf_set_freq(device, frequency);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_freq() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Frequency set: %d\r\n", (int)frequency);
}

void HackRFDevice::setLnaGain(uint32_t gain) {
	hackrf_error result = (hackrf_error)hackrf_set_lna_gain(device, gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_lna_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Gain LNA set: %d\r\n", (int)gain);
}

void HackRFDevice::setVgaGain(uint32_t gain) {
	hackrf_error result = (hackrf_error)hackrf_set_vga_gain(device, gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_vga_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Gain VGA set: %d\r\n", (int)gain);
}

void HackRFDevice::setTxVgaGain(uint32_t gain) {
	hackrf_error result = (hackrf_error)hackrf_set_txvga_gain(device, gain);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_txvga_gain() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("Gain tx VGA set: %d\r\n", (int)gain);
}

void HackRFDevice::setBaseband(int baseband) {
	hackrf_error result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_baseband_filter_bandwidth() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
}

void HackRFDevice::enableAmp(uint8_t amp) {
	hackrf_error result = (hackrf_error)hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"hackrf_set_amp_enable() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
	}
	printf("AMP enabled set: %d\r\n", (int)amp);
}

void HackRFDevice::setDataForTransmitting(TransmittingData* transmittingData) {
	this->transmittingData = transmittingData;
}

bool HackRFDevice::isDeviceTransmitting() {
	return isTxOn;
}

HackRFDevice::~HackRFDevice() {
	printf("~HackRFDevice()\r\n");
	stop();
	delete bufferForSpec;
	delete bufferForProc;
}

Result HackRFDevice::start() {
	isTxOn = false;
	uint8_t amp = config->hackrf.rxAmp;
	uint8_t antenna = 0;
	uint32_t baseband = config->hackrf.basebandFilter;

	//0, 8, 16, 24, 32, 40 
	uint32_t lna_gain = config->hackrf.lnaGain * 8;

	//0 .. 62 step 2
	uint32_t vga_gain = config->hackrf.vgaGain * 2;

	//0 .. 47 step 1
	uint32_t txVgaGain = config->hackrf.txVgaGain;

	hackrf_error result;

	Result initResult = { CREATED_BUT_NOT_INIT, std::string("") };

	result = (hackrf_error)hackrf_init();
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_init() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	const char* serial_number = NULL;
	//result = (hackrf_error)hackrf_open_by_serial(serial_number, &device);
	result = (hackrf_error)hackrf_open(&device);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_open() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_hw_sync_mode(device, (uint8_t)0);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_hw_sync_mode() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_sample_rate(device, config->hackrf.deviceSamplingRate);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_sample_rate() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	int64_t freq_hz = config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);

	result = (hackrf_error)hackrf_set_freq(device, freq_hz);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_freq() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_amp_enable(device, amp);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_amp_enable() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_antenna_enable(device, antenna);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_antenna_enable() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_vga_gain(device, vga_gain);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_vga_gain() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}
	result = (hackrf_error)hackrf_set_lna_gain(device, lna_gain);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_lna_gain() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_baseband_filter_bandwidth(device, hackrf_compute_baseband_filter_bw(baseband));
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_baseband_filter_bandwidth() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_set_txvga_gain(device, txVgaGain);
	if (result != HACKRF_SUCCESS) {
		initResult.err.append("hackrf_set_txvga_gain() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.status = INIT_FAULT;
		return initResult;
	}

	result = (hackrf_error)hackrf_start_rx(device, rx_callback, this);
	if (result != HACKRF_SUCCESS) {
		initResult.status = INIT_FAULT;
		return initResult;
		initResult.err.append("hackrf_start_rx() failed: ");
		initResult.err.append(hackrf_error_name(result));
		initResult.err.append("\n Error code: ");
		initResult.err.append(std::to_string(result));
	} else {
		initResult.status = INIT_OK;
	}

	return initResult;

	if (DEBUG) printf("HackRFDevice::init()\r\n");
}

bool HackRFDevice::startTX() {
	if (device != NULL) {
		hackrf_error result = (hackrf_error)hackrf_start_tx(device, tx_callback, this);
		if (result != HACKRF_SUCCESS) {
			if (DEBUG) printf("Error hackrf_start_tx!\r\n");
		} else {
			isTxOn = true;
			config->TRANSMITTING = true;
			return true;
		}
	}
	return false;
}

bool HackRFDevice::stopTX() {
	if (device != NULL) {
		if (hackrf_is_streaming(device)) {
			hackrf_error result = (hackrf_error)hackrf_stop_tx(device);
			if (result != HACKRF_SUCCESS) {
				if (DEBUG) printf("Error hackrf_stop_tx!\r\n");
			}
			else {
				isTxOn = false;
				config->TRANSMITTING = false;
				return true;
			}
		}
	}
	return false;
}

bool HackRFDevice::pauseRX() {
	if (device != NULL) {
		if (hackrf_is_streaming(device)) {
			hackrf_error result = (hackrf_error)hackrf_stop_rx(device);
			if (result != HACKRF_SUCCESS) {
				if (DEBUG) printf("Error hackrf_stop_rx!\r\n");
			} else {
				if (DEBUG) printf("RX paused!\r\n");
				return true;
			}
		}
	}
	return false;
}

bool HackRFDevice::releasePauseRX() {
	hackrf_error result = (hackrf_error)hackrf_start_rx(device, rx_callback, this);
	if (result != HACKRF_SUCCESS) {
		if (DEBUG) printf("Error hackrf_start_rx!\r\n");
	} else return true;
	return false;
}

void HackRFDevice::stop() {
	hackrf_error result = HACKRF_ERROR_OTHER;
	if (device != NULL) {
		if (isDeviceTransmitting()) {
			hackrf_error result = (hackrf_error)hackrf_stop_tx(device);
			if (result != HACKRF_SUCCESS) {
				if (DEBUG) printf("Error hackrf_stop_tx!\r\n");
			}
		}
		
		hackrf_error result = (hackrf_error)hackrf_stop_rx(device);
		if (result != HACKRF_SUCCESS) {
			if (DEBUG) printf("Error hackrf_stop_rx!\r\n");
		}
		
		result = (hackrf_error)hackrf_close(device);
		if (result != HACKRF_SUCCESS) {
			if (DEBUG) printf("Error hackrf_close!\r\n");
		}
		hackrf_exit();
		if (DEBUG) printf("Device stopped!\r\n");
		isTxOn = false;
	}
}

CircleBufferNew<uint8_t>* HackRFDevice::getBufferForSpec() {
	return bufferForSpec;
}

CircleBufferNew<uint8_t>* HackRFDevice::getBufferForProc() {
	return bufferForProc;
}

float HackRFDevice::prepareData(uint8_t val) {
	val ^= (uint8_t)0x80;
	return (((float)val / 130.0f) - 1.0f);
}