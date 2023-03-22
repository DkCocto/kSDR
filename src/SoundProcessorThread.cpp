#include "SoundProcessorThread.h"

SoundProcessorThread::SoundProcessorThread(DeviceController* devCnt, 
											ViewModel* viewModel, 
											ReceiverLogicNew* receiverLogicNew, 
											Config* config, 
											CircleBuffer* iqSignalsCircleBuffer, 
											CircleBuffer* sWCB, 
											FFTSpectreHandler* fftSpectreHandler) {
	this->config = config;
	this->devCnt = devCnt;
	this->viewModel = viewModel;
	this->receiverLogicNew = receiverLogicNew;

	mixer = new Mixer(config->inputSamplerate);

	len = config->readSoundProcessorBufferLen;

	this->iqSignalsCircleBuffer = iqSignalsCircleBuffer;
	this->soundWriterCircleBuffer = sWCB;
	this->fftSpectreHandler = fftSpectreHandler;

	hilbertTransform = new HilbertTransform(config->inputSamplerate, config->hilbertTransformLen);
	delay = new Delay((config->hilbertTransformLen - 1) / 2);
	agc = new AGC(config, fftSpectreHandler);

	decimateBufferI = new double[config->outputSamplerateDivider];
	memset(decimateBufferI, 0, sizeof(double) * config->outputSamplerateDivider);

	decimateBufferQ = new double[config->outputSamplerateDivider];
	memset(decimateBufferQ, 0, sizeof(double) * config->outputSamplerateDivider);

	//Инициализация полифазных фильтров
	initFilters(config->defaultFilterWidth);

	//audioFilterFM.init(audioFilterFM.LOWPASS, audioFilterFM.BARTLETT, 256, 200000, 0, config->outputSamplerate);
}

void SoundProcessorThread::initFilters(int filterWidth) {
	firFilterI.initCoeffs(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	firFilterQ.initCoeffs(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	fir->init(fir->LOWPASS, fir->BLACKMAN_HARRIS, 512, filterWidth, 0, config->outputSamplerate);

	firI->init(fir->LOWPASS, fir->BARTLETT, 256, filterWidth, 0, config->outputSamplerate);
	firQ->init(fir->LOWPASS, fir->BARTLETT, 256, filterWidth, 0, config->outputSamplerate);
}

void SoundProcessorThread::process() {
	outputData = new float[(len / 2) / config->outputSamplerateDivider];

	int storedFilterWidth = config->defaultFilterWidth;

	short decimationCount = 0;

	float* data = new float[len];

	FMDemodulator fmDemodulator;

	isWorking_ = true;

	while (true) {

		if (!config->WORKING) {
			isWorking_ = false;
			break;
		}

		if (!devCnt->isReadyToReceiveCmd()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		};

		//Обработка ширины фильтра
		if (storedFilterWidth != viewModel->filterWidth) {
			storedFilterWidth = viewModel->filterWidth;
			initFilters(storedFilterWidth);
		}
		//------------------------

		int available = iqSignalsCircleBuffer->available();
		viewModel->setBufferAvailable(available);
		if (available >= len) {
			iqSignalsCircleBuffer->read(data, len);

			long count = 0;
			for (int i = 0; i < len / 2; i++) {

				if (viewModel->removeDCBias) dcRemove.process(&data[2 * i], &data[2 * i + 1]);

				mixer->setFreq(receiverLogicNew->getFrequencyDelta());
				Signal mixedSignal = mixer->mix(data[2 * i], data[2 * i + 1]);

				decimateBufferI[decimationCount] = mixedSignal.I;
				decimateBufferQ[decimationCount] = mixedSignal.Q;

				decimationCount++;

				if (i % config->outputSamplerateDivider == 0) {

					decimationCount = 0;

					double audioI = firFilterI.filter(decimateBufferI, config->outputSamplerateDivider);
					double audioQ = firFilterQ.filter(decimateBufferQ, config->outputSamplerateDivider);

					int mode = USB;

					mode = viewModel->receiverMode;
					//mode = FM;

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
							audioI = firI->proc(audioI);
							audioQ = firQ->proc(audioQ);
							audio = sqrt(audioI * audioI + audioQ * audioQ);
							break;
						case nFM:
							audioI = firI->proc(audioI);
							audioQ = firQ->proc(audioQ);
							audio = fmDemodulator.demodulate(audioI, audioQ);
							//audio = audioFilterFM.proc(audio);
							break;
					}
					//-------------------audio = audioFilter->filter(audio);
					audio = fir->proc(audio);
					//audio = agc->process(audio);
					audio = agc->processNew(audio);
					//Если AM, то немного усилим сигнал
					if (mode == AM) audio *= 3.0f;
					if (mode == nFM) audio *= 2.0f;
					outputData[count] = audio * viewModel->volume;
					count++;
				}
		
			}

			soundWriterCircleBuffer->write(outputData, (len / 2) / config->outputSamplerateDivider);
			//fftSpectreHandler->setSpectreSpeed(Display::instance->viewModel->spectreSpeed);
			fftSpectreHandler->putData(data);
		} else {
			//printf("SoundProcessorThread: Waiting for iqSignalsCircleBuffer...\r\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	printf("SoundProcessStopped\r\n");
	isWorking_ = false;
}

std::thread SoundProcessorThread::start() {
	std::thread p(&SoundProcessorThread::process, this);
	DWORD result = ::SetThreadIdealProcessor(p.native_handle(), 2);
	SetThreadPriority(p.native_handle(), THREAD_PRIORITY_HIGHEST);
	return p;
}

bool SoundProcessorThread::isWorking() {
	return isWorking_;
}
