#include "SoundProcessorThread.h"

SoundProcessorThread::SoundProcessorThread(Config* config, CircleBuffer* iqSignalsCircleBuffer, CircleBuffer* sWCB, FFTSpectreHandler* fftSpectreHandler) {
	this->config = config;

	mixer = new Mixer(config->inputSamplerate);

	len = config->readSoundProcessorBufferLen;

	this->iqSignalsCircleBuffer = iqSignalsCircleBuffer;
	this->soundWriterCircleBuffer = sWCB;
	this->fftSpectreHandler = fftSpectreHandler;

	hilbertTransform = new HilbertTransform(config->inputSamplerate, config->hilbertTransformLen);
	delay = new Delay((config->hilbertTransformLen - 1) / 2);
	agc = new AGC(fftSpectreHandler);

	//Инициализация полифазных фильтров
	initFilters(config->defaultFilterWidth);

	decimateBufferI = new double[config->outputSamplerateDivider];
	decimateBufferQ = new double[config->outputSamplerateDivider];
}

void SoundProcessorThread::initFilters(int filterWidth) {
	firFilterI = new PolyPhaseFilter(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	firFilterQ = new PolyPhaseFilter(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	audioFilter = new FirFilter(Filter::makeRaiseCosine(config->outputSamplerate, filterWidth, 1, config->polyphaseFilterLen), config->polyphaseFilterLen);
}

float xm1 = 0, ym1 = 0, xm2 = 0, ym2 = 0;
float p1 = 0, p2 = 0;

void SoundProcessorThread::process() {
	outputData = new float[(len / 2) / config->outputSamplerateDivider];

	int storedFilterWidth = config->defaultFilterWidth;

	int decimateCount = 1;
	long decimationCount = 0;

	float* data;

	while (true) {

		int available = iqSignalsCircleBuffer->available();
		if (available >= len) {
			data = iqSignalsCircleBuffer->read(len);

			//Обработка ширины фильтра
			if (storedFilterWidth != Display::instance->viewModel->filterWidth) {
				storedFilterWidth = Display::instance->viewModel->filterWidth;
				initFilters(storedFilterWidth);
			}
			//------------------------

			long count = 0;
			for (int i = 0; i < len / 2; i++) {

				p1 = data[2 * i];
				data[2 * i] = p1 - xm1 + 0.9995 * ym1;
				xm1 = p1;
				ym1 = data[2 * i];

				p2 = data[2 * i + 1];
				data[2 * i + 1] = p2 - xm2 + 0.9995 * ym2;
				xm2 = p2;
				ym2 = data[2 * i + 1];

				mixer->setFreq(Display::instance->spectre->receiverLogicNew->getFrequencyDelta());
				Signal mixedSignal = mixer->mix(data[2 * i], data[2 * i + 1]);

				//Уменьшаем силу выходного сигнала после смесителя, чтобы фильтры не перегружались от сильных сигналов
				//if (Display::instance->viewModel->att) {
					mixedSignal.I *= 0.01;
					mixedSignal.Q *= 0.01;
				//}

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
					audio = audioFilter->filter(audio);
					audio = agc->process(audio) * Display::instance->viewModel->volume;
					//Если AM, то немного усилим сигнал
					if (mode == AM) audio *= 7;
					outputData[count] = audio;
					count++;
				}
		
			}

			soundWriterCircleBuffer->write(outputData, (len / 2) / config->outputSamplerateDivider);
			fftSpectreHandler->setSpectreSpeed(Display::instance->viewModel->spectreSpeed);
			fftSpectreHandler->putData(data);
			delete data;
		} else {
			//printf("SoundProcessorThread: Waiting for iqSignalsCircleBuffer...\r\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

std::thread SoundProcessorThread::start() {
	std::thread p(&SoundProcessorThread::process, this);
	return p;
}