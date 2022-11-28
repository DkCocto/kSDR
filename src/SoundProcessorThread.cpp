#include "SoundProcessorThread.h"

SoundProcessorThread::SoundProcessorThread(Config* config, CircleBuffer* sPCB, CircleBuffer* sWCB, FFTSpectreHandler* fftSpectreHandler) {
	this->config = config;

	mixer = new Mixer(config->inputSamplerate);

	len = config->readSoundProcessorBufferLen;

	soundProcessorCircleBuffer = sPCB;
	soundWriterCircleBuffer = sWCB;
	this->fftSpectreHandler = fftSpectreHandler;

	hilbertTransform = new HilbertTransform(config->inputSamplerate, config->hilbertTransformLen);
	delay = new Delay((config->hilbertTransformLen - 1) / 2);
	agc = new AGC(fftSpectreHandler);


	//Инициализация полифазных фильтров
	initFilters(config->defaultFilterWidth);

	decimateBufferI = new double[config->outputSamplerateDivider];
	decimateBufferQ = new double[config->outputSamplerateDivider];
}

void SoundProcessorThread::putData(float* data, int len) {
	soundProcessorCircleBuffer->write(data, len);
}

void SoundProcessorThread::initFilters(int filterWidth) {
	firFilterI = new PolyPhaseFilter(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	firFilterQ = new PolyPhaseFilter(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	audioFilter = new FirFilter(Filter::makeRaiseCosine(config->outputSamplerate, filterWidth, 1, config->polyphaseFilterLen), config->polyphaseFilterLen);
}

void SoundProcessorThread::process() {
	outputData = new float[(len / 2) / config->outputSamplerateDivider];

	int storedFilterWidth = config->defaultFilterWidth;

	int decimateCount = 1;
	long decimationCount = 0;

	float* data;

	while (true) {

		int available = soundProcessorCircleBuffer->available();
		if (available >= len) {
			data = soundProcessorCircleBuffer->read(len);

			fftSpectreHandler->setSpectreSpeed(Display::instance->viewModel->spectreSpeed);
			//if (!fftSpectreHandler->getSemaphore()->isLocked()) {
			fftSpectreHandler->putData(data, len);
			//}

			//Обработка ширины фильтра
			if (storedFilterWidth != Display::instance->viewModel->filterWidth) {
				storedFilterWidth = Display::instance->viewModel->filterWidth;
				initFilters(storedFilterWidth);
			}
			//------------------------

			long count = 0;
			for (int i = 0; i < len / 2; i++) {

				mixer->setFreq(Display::instance->spectre->receiverLogicNew->getSelectedFreq());
				Signal mixedSignal = mixer->mix(data[2 * i], data[2 * i + 1]);

				//printf("%f\r\n", mixedSignal.Q);

				decimateBufferI[decimationCount] = mixedSignal.I;
				decimateBufferQ[decimationCount] = mixedSignal.Q;

				decimationCount++;

				if (i % config->outputSamplerateDivider == 0) {
					decimationCount = 0;
					double audioI = firFilterI->filter(decimateBufferI, config->outputSamplerateDivider);
					double audioQ = firFilterQ->filter(decimateBufferQ, config->outputSamplerateDivider);

					int mode = USB;

					ViewModel* viewModel = Display::instance->viewModel;

					mode = viewModel->receiverMode;

					double audio = 0;

					switch (mode) {
					case USB:
						audioQ = hilbertTransform->filter(audioQ);
						audioI = delay->filter(audioI);
						audio = audioI - audioQ; // LSB
						break;
					case LSB:
						audioQ = hilbertTransform->filter(audioQ);
						audioI = delay->filter(audioI);
						audio = audioI + audioQ; // USB
						break;
					case AM:
						audio = sqrt(audioI * audioI + audioQ * audioQ);
						break;
					}
					if (viewModel->audioFilter) audio = audioFilter->filter(audio);
					audio = agc->process(audio) * Display::instance->viewModel->volume;
					outputData[count] = audio;
					count++;
				}
		
			}

			//soundCard->write(outputData, (len / 2) / DECIMATION_LEVEL);
			//printf("%i\r\n", (len / 2) / DECIMATION_LEVEL);
			soundWriterCircleBuffer->write(outputData, (len / 2) / config->outputSamplerateDivider);
			delete data;
		} else {
			//printf("SUKA!\r\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

std::thread SoundProcessorThread::start() {
	std::thread p(&SoundProcessorThread::process, this);
	return p;
}