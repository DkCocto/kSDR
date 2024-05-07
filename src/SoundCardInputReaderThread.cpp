#include "SoundCardInputReaderThread.h"

void SoundCardInputReaderThread::initFilters(int audioFilterWidth) {
	if (this->audioFilterWidth != audioFilterWidth) {
		this->audioFilterWidth = audioFilterWidth;
		this->resamplingFilterWidth = 2 * audioFilterWidth;
		audioFilter.init(audioFilter.LOWPASS, audioFilter.BLACKMAN, 64, this->audioFilterWidth, 0, config->inputSamplerateSound);
		//resamplingFilter.init(resamplingFilter.LOWPASS, resamplingFilter.BLACKMAN, 199, this->resamplingFilterWidth, 0, config->currentWorkingInputSamplerate);
		//resamplingFilter2.init(resamplingFilter.LOWPASS, resamplingFilter.BLACKMAN, 199, 6000, 0, config->currentWorkingInputSamplerate);
		
		if (fastfir != nullptr) delete fastfir;
		//create FastFIR filter
		fastfir = new JFastFIRFilter;

		//set the kernel of the filter, in this case a Low Pass filter at 800Hz
		fastfir->setKernel(JFilterDesign::LowPassHanning(this->resamplingFilterWidth, config->currentWorkingInputSamplerate, 511));
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

void SoundCardInputReaderThread::pause() {
	CurrentStatus = PAUSE;
}

void SoundCardInputReaderThread::continueRead() {
	CurrentStatus = START_READING;
}

void SoundCardInputReaderThread::run() {

	isWorking_ = true;

	const int BUFFER_READ_LEN = config->audioReadFrameLen; //2

	int relation = config->currentWorkingInputSamplerate / config->inputSamplerateSound; // 4 000 000 / 31 250 = 128
	int upsamplingDataLen = BUFFER_READ_LEN * relation; // 2 * 128 = 256

	float* readBuffer = new float[BUFFER_READ_LEN];

	float *upsamplingData = new float[upsamplingDataLen];
	memset(upsamplingData, 0, sizeof(float) * upsamplingDataLen);

	vector<double> data;
	data.reserve(upsamplingDataLen);

	//int fftBufferLen = upsamplingDataLen;

	//if (fftBuffer != nullptr) delete[] fftBuffer;
	//fftBuffer = new fftw_complex[fftBufferLen]; //16 * 256 = 4096

	//fftw_complex* fftBuffer = new fftw_complex[fftBufferLen]; // 256

	//fftw_complex* fftBufferOut = new fftw_complex[4]; //16 * 256 = 4096

	//planForward = fftw_plan_dft_1d(fftBufferLen, fftBuffer, fftBuffer, FFTW_FORWARD, FFTW_MEASURE);
	//planBackward = fftw_plan_dft_1d(fftBufferLen, fftBuffer, fftBuffer, FFTW_BACKWARD, FFTW_MEASURE);

	//memset(upsamplingData, 0, sizeof(float) * upsamplingDataLen);

	while (true) {
		if (!config->WORKING) {
			printf("SoundCardInputReaderThread Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (CurrentStatus == START_READING) {
			soundCard->startInput();
			CurrentStatus = READING;
		}

		if (CurrentStatus == PAUSE) {
			soundCard->stopInput();
			CurrentStatus = REST;
			continue;
		}

		if (CurrentStatus == REST) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

			for (int i = 0; i < BUFFER_READ_LEN; i++) {
				float audio = audioFilter.proc(readBuffer[i]) * 6.0f;
				upsamplingData[i * relation] = audio;
			}

			for (int i = 0; i < upsamplingDataLen; i++) {
				if (i % relation != 0) {
					float dither = ((float)rand() / (float)(RAND_MAX)) / 500000.0f;
					upsamplingData[i] = dither;
				}
				//upsamplingData[i] = resamplingFilter.proc(upsamplingData[i]) * 5.0f;
			}

			std::vector<double> v(upsamplingData, upsamplingData + upsamplingDataLen);

			fastfir->Update(v);
			
			for (int i = 0; i < upsamplingDataLen; i++) {
				upsamplingData[i] = v[i] * 6.0f;
			}

			soundInputBuffer->write(upsamplingData, upsamplingDataLen);

		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		/*int available = soundWriterCircleBuffer->available();
		secondsInBuffer = (float)available / config->outputSamplerate;

		//Если начинается задержка в звуке, то сбросить буффер. Это ликвидирует задержку в звуке.
		if (secondsInBuffer > 0.15) {
			soundWriterCircleBuffer->reset();
			continue;
		}*/

		/*if (available >= len) {
			soundWriterCircleBuffer->read(data, len);
			soundCard->write(data, len);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}*/
	}
	delete[] readBuffer;
	delete[] upsamplingData;
}