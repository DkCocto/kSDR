#include "SoundCardInputReaderThread.h"

void SoundCardInputReaderThread::initFilters(int audioFilterWidth) {
	if (this->audioFilterWidth != audioFilterWidth) {
		this->audioFilterWidth = audioFilterWidth;
		this->resamplingFilterWidth = 2 * audioFilterWidth;
		audioFilter.init(audioFilter.LOWPASS, audioFilter.BLACKMAN, 64, this->audioFilterWidth, 0, config->inputSamplerateSound);
		
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

	int relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound; // 4 000 000 / 31 250 = 128
	int upsamplingDataLen = BUFFER_READ_LEN * relation; // 2 * 128 = 256

	float* readBuffer = new float[BUFFER_READ_LEN];

	float *upsamplingDataOut = new float[upsamplingDataLen];
	memset(upsamplingDataOut, 0, sizeof(float) * upsamplingDataLen);

	std::vector<double> upsamplingData(upsamplingDataLen);
	upsamplingData.reserve(upsamplingDataLen);

	vector<double> data;
	data.reserve(upsamplingDataLen);

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
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		initFilters(config->filterWidth);

		//BUFFER_READ_LEN = 2
		//samplerate = 4000000
		//input samplerate = 31250
		//relation = 4000000 / 31250 = 128
		//upsamplingDataLen = 2 * 128 = 256

		if (soundCard->availableToRead() >= BUFFER_READ_LEN) {
			soundCard->read(readBuffer, BUFFER_READ_LEN);

			for (int i = 0; i < upsamplingDataLen; i++) {
				//if (i % relation != 0) {
				double dither = ((double)rand() / (double)(RAND_MAX)) / 100000.0;
				upsamplingData[i] = dither;
				//}
			}

			for (int i = 0; i < BUFFER_READ_LEN; i++) {
				float audio = audioFilter.proc(readBuffer[i] * config->transmit.inputLevel);
				upsamplingData[i * relation] = audio;
			}

			fastfir->Update(upsamplingData);
			
			//std::copy(v.begin(), v.end(), upsamplingData);

			for (int i = 0; i < upsamplingDataLen; i++) {
				upsamplingDataOut[i] = upsamplingData[i] * config->transmit.inputLevel;
			}

			soundInputBuffer->write(upsamplingDataOut, upsamplingDataLen);

		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	delete[] readBuffer;
	delete[] upsamplingDataOut;
}