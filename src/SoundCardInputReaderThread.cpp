#include "SoundCardInputReaderThread.h"

void SoundCardInputReaderThread::initFilters(int audioFilterWidth) {
	if (this->audioFilterWidth != audioFilterWidth) {
		this->audioFilterWidth = audioFilterWidth;
		this->resamplingFilterWidth = audioFilterWidth;
		audioFilter.init(audioFilter.LOWPASS, audioFilter.BLACKMAN_NUTTAL, 64, this->audioFilterWidth, 0, config->inputSamplerateSound);
		
		if (fastfir != nullptr) delete fastfir;
		//create FastFIR filter
		fastfir = new JFastFIRFilter;

		//set the kernel of the filter, in this case a Low Pass filter at 800Hz
		fastfir->setKernel(JFilterDesign::LowPassHanning(this->resamplingFilterWidth, config->currentWorkingInputSamplerate, 1111));
	}

}

SoundCardInputReaderThread::SoundCardInputReaderThread(Config* config, CircleBufferNew<float>* soundInputBuffer, SoundCard* soundCard) {
	this->config = config;
	this->soundInputBuffer = soundInputBuffer;
	this->soundCard = soundCard;

	initFilters(config->defaultFilterWidth);
}

SoundCardInputReaderThread::~SoundCardInputReaderThread() {
	if (fastfir != nullptr) delete fastfir;
}

std::thread SoundCardInputReaderThread::start() {
	std::thread p(&SoundCardInputReaderThread::run, this);
	return p;
}

void SoundCardInputReaderThread::run() {

	isWorking_ = true;

	const int BUFFER_READ_LEN = config->audioReadFrameLen; //2

	int relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound; // 8 000 000 / 31 250 = 256
	int upsamplingDataLen = BUFFER_READ_LEN * relation; // 32 * 256 = 8192

	float* readBuffer = new float[BUFFER_READ_LEN]; // count = 2

	float *upsamplingDataOut = new float[upsamplingDataLen];
	memset(upsamplingDataOut, 0, sizeof(float) * upsamplingDataLen);

	std::vector<double> upsamplingData(upsamplingDataLen);
	upsamplingData.reserve(upsamplingDataLen);

	vector<double> data;
	data.reserve(upsamplingDataLen);

	CosOscillator so(TONE_SIGNAL_FREQ, config->inputSamplerateSound);

	while (true) {
		if (!config->WORKING) {
			printf("SoundCardInputReaderThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (currentStatus == START_READING) {
			soundCard->startInput();
			currentStatus = READING;
		}

		if (currentStatus == PAUSE) {
			soundCard->stopInput();
			currentStatus = REST;
			continue;
		}

		if (currentStatus == REST) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}

		initFilters(config->filterWidth);

		//BUFFER_READ_LEN = 2
		//samplerate = 4000000
		//input samplerate = 40000
		//relation = 4000000 / 40000 = 100
		//upsamplingDataLen = 2 * 100 = 200

		if (soundCard->availableToRead() >= BUFFER_READ_LEN) {
			soundCard->read(readBuffer, BUFFER_READ_LEN);

			if (config->transmit.sendToneSignal) {
				for (int i = 0; i < BUFFER_READ_LEN; i++) {
					readBuffer[i] = so.nextSample();
				}
			}

			for (int i = 0; i < upsamplingDataLen; i++) {
				if (i % relation != 0) {
					double dither = ((double)rand() / (double)(RAND_MAX)) / 10000000;
					upsamplingData[i] = dither;
				}
			}

			for (int i = 0; i < BUFFER_READ_LEN; i++) {
				float audio = audioFilter.proc(readBuffer[i] * config->transmit.inputLevel);
				upsamplingData[i * relation] = audio;
			}

			fastfir->Update(upsamplingData);
			
			std::copy(upsamplingData.begin(), upsamplingData.end(), upsamplingDataOut);

			/*for (int i = 0; i < upsamplingDataLen; i++) {
				upsamplingDataOut[i] = upsamplingData[i];
			}*/

			soundInputBuffer->write(upsamplingDataOut, upsamplingDataLen);

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	delete[] readBuffer;
	delete[] upsamplingDataOut;
}