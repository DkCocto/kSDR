#include "SoundProcessorThread.h"
#include "CircleBufferNew.h"

#include "device/HackRFDevice.h"
#include "device/RTLDevice.h"
#include "device/RSPDevice.h"

SoundProcessorThread::SoundProcessorThread(DeviceController* devCnt, 
											ViewModel* viewModel, 
											ReceiverLogic* receiverLogic, 
											Config* config, 
											CircleBufferNew<float>* soundWriterCircleBuffer,
											SpectreHandler* specHandler) {
	this->config = config;
	this->devCnt = devCnt;
	this->viewModel = viewModel;
	this->receiverLogic = receiverLogic;

	mixer = Mixer(config->inputSamplerate);

	len = config->readSoundProcessorBufferLen;

	this->soundWriterCircleBuffer = soundWriterCircleBuffer;
	this->specHandler = specHandler;

	hilbertTransform = new HilbertTransform(config->inputSamplerate, config->hilbertTransformLen);
	delay = new Delay((config->hilbertTransformLen - 1) / 2);
	agc = AGC(config, specHandler);

	decimateBufferI = new float[config->outputSamplerateDivider];
	memset(decimateBufferI, 0, sizeof(float) * config->outputSamplerateDivider);

	decimateBufferQ = new float[config->outputSamplerateDivider];
	memset(decimateBufferQ, 0, sizeof(float) * config->outputSamplerateDivider);

	outputData = new float[(len / 2) / config->outputSamplerateDivider];

	//������������� ���������� ��������
	initFilters(config->defaultFilterWidth);
}

SoundProcessorThread::~SoundProcessorThread() {
	printf("~SoundProcessorThread()\r\n");
	delete[] decimateBufferI;
	delete[] decimateBufferQ;
	delete[] outputData;

	delete delay;
	delete hilbertTransform;
}

void SoundProcessorThread::initFilters(int filterWidth) {
	firFilterI.initCoeffs(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	firFilterQ.initCoeffs(config->inputSamplerate, filterWidth, config->outputSamplerateDivider, config->polyphaseFilterLen);
	fir.init(fir.LOWPASS, fir.BLACKMAN_HARRIS, 512, filterWidth, 0, config->outputSamplerate);

	firI.init(fir.LOWPASS, fir.BARTLETT, 256, filterWidth, 0, config->outputSamplerate);
	firQ.init(fir.LOWPASS, fir.BARTLETT, 256, filterWidth, 0, config->outputSamplerate);
}

void SoundProcessorThread::run() {
	isWorking_ = true;

	int storedFilterWidth = config->defaultFilterWidth;

	DeviceN* device = devCnt->getDevice();
	DeviceType deviceType = devCnt->getCurrentDeviceType();

	while (true) {
		if (!config->WORKING) {
			printf("SoundProcess Stopped\r\n");
			isWorking_ = false;
			return;
		}

		//��������� ������ �������
		if (storedFilterWidth != viewModel->filterWidth) {
			storedFilterWidth = viewModel->filterWidth;
			initFilters(storedFilterWidth);
		}
		//------------------------

		
		if (device != nullptr) {
			switch (deviceType) {
				case HACKRF:
					initProcess<HackRFDevice, uint8_t>((HackRFDevice*)device);
					break;
				case RTL:
					initProcess<RTLDevice, unsigned char>((RTLDevice*)device);
					break;
				case RSP:
					initProcess<RSPDevice, short>((RSPDevice*)device);
					break;
			}
		}
	}
}

template<typename DEVICE, typename DATATYPE> void SoundProcessorThread::initProcess(DEVICE* device) {
	auto buffer = device->getBufferForProc();
	int available = buffer->available();
	viewModel->setBufferAvailable(available);
	if (available >= len) {
		processData<DATATYPE, DEVICE>(buffer->read(), device);
	} else std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

template<typename T, typename D> void SoundProcessorThread::processData(T* data, D* device) {
	long count = 0;
	short decimationCount = 0;
	for (int i = 0; i < len / 2; i++) {

		float I = device->prepareData(data[2 * i]);
		float Q = device->prepareData(data[2 * i + 1]);

		dcRemover.process(&I, &Q);

		mixer.setFreq(receiverLogic->getFrequencyDelta());
		Signal mixedSignal = mixer.mix(I, Q);

		decimateBufferI[decimationCount] = mixedSignal.I;
		decimateBufferQ[decimationCount] = mixedSignal.Q;

		decimationCount++;

		if (i % config->outputSamplerateDivider == 0) {

			decimationCount = 0;

			float audioI = firFilterI.filter(decimateBufferI, config->outputSamplerateDivider);
			float audioQ = firFilterQ.filter(decimateBufferQ, config->outputSamplerateDivider);

			int mode = viewModel->receiverMode;

			float audio = 0;

			switch (mode) {
				case DSB:
					audio = audioI + audioQ;
					break;
				case USB:
					audioQ = hilbertTransform->filter(audioQ);
					audioI = delay->filter(audioI);
					audio = audioI - audioQ; // USB
					break;
				case LSB:
					audioQ = hilbertTransform->filter(audioQ);
					audioI = delay->filter(audioI);
					audio = audioI + audioQ; // LSB
					break;
				case AM:
					audioI = firI.proc(audioI);
					audioQ = firQ.proc(audioQ);
					audio = sqrt(audioI * audioI + audioQ * audioQ);
					break;
				case nFM:
					audioI = firI.proc(audioI);
					audioQ = firQ.proc(audioQ);
					audio = fmDemodulator.demodulate(audioI, audioQ);
					break;
			}
			//-------------------
			audio = fir.proc(audio);
			audio = agc.processNew(audio);
			//���� AM, �� ������� ������ ������
			if (mode == AM) audio *= 3.0f;
			if (mode == nFM) audio *= 2.0f;
			outputData[count] = audio * viewModel->volume;
			count++;
		}

	}

	soundWriterCircleBuffer->write(outputData, (len / 2) / config->outputSamplerateDivider);
}

std::thread SoundProcessorThread::start() {
	std::thread p(&SoundProcessorThread::run, this);
	//DWORD result = ::SetThreadIdealProcessor(p.native_handle(), 2);
	//SetThreadPriority(p.native_handle(), THREAD_PRIORITY_HIGHEST);
	return p;
}