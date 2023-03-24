#include "Environment.h"

Environment::Environment() {
	config = new Config();
	deviceController = new DeviceController(config);
	IQSourceBuffer = new CircleBuffer(config->circleBufferLen);
	deviceController->addReceiver(IQSourceBuffer);
	soundBuffer = new CircleBuffer(config->circleBufferLen);
	fftData = new FFTData(config->fftLen / 2);
	fftSpectreHandler = new FFTSpectreHandler(config, fftData);
	viewModel = new ViewModel(config);
	flowingFFTSpectre = new FlowingFFTSpectre(config, viewModel, fftSpectreHandler);
	receiverLogicNew = new ReceiverLogic(config, viewModel, flowingFFTSpectre);
}

Environment::~Environment() {
	delete config;
	/*delete IQSourceBuffer;
	delete deviceController;
	delete fftSpectreHandler;
	delete soundBuffer;
	delete soundProcessor;
	delete circleBufferWriterThread;
	delete soundCard;
	delete viewModel;
	delete flowingFFTSpectre;
	delete receiverLogicNew;*/
}

Config* Environment::getConfig() {
	return config;
}

DeviceController* Environment::getDeviceController() {
	return deviceController;
}

CircleBuffer* Environment::getIQSourceBuffer() {
	return IQSourceBuffer;
}

void Environment::init() {
	soundCard = new SoundCard(config);
	soundProcessor = new SoundProcessorThread(deviceController, viewModel, receiverLogicNew, config, IQSourceBuffer, soundBuffer, fftSpectreHandler);
	circleBufferWriterThread = new CircleBufferWriterThread(config, deviceController, soundBuffer, soundCard);
}

void Environment::startProcessing() {
	deviceController->start(config->deviceType);

	if (deviceController->isReadyToReceiveCmd()) {
		config->WORKING = true;
		
		init();

		fftSpectreHandler->start().detach();

		//Инициализируем звуковую карту
		soundCard->open();

		circleBufferWriterThread->start().detach();
		soundProcessor->start().detach();
	}
}

void Environment::stopProcessing() {
	deviceController->forceStop();

	//Stop 3 threads: sound process, soundcard writer, fft handler
	config->WORKING = false; 

	while (soundProcessor->isWorking() || circleBufferWriterThread->isWorking() || fftSpectreHandler->isWorking()) {}

	delete soundProcessor;
	soundProcessor = nullptr;
	
	delete circleBufferWriterThread;
	circleBufferWriterThread = nullptr;
	
	delete soundCard;
	soundCard = nullptr;
}

FFTSpectreHandler* Environment::getFFTSpectreHandler() {
	return fftSpectreHandler;
}

CircleBuffer* Environment::getSoundBuffer() {
	return soundBuffer;
}

SoundProcessorThread* Environment::getSoundProcessor() {
	return soundProcessor;
}

SoundCard* Environment::getSoundCard() {
	return soundCard;
}

ViewModel* Environment::getViewModel() {
	return viewModel;
}

ReceiverLogic* Environment::getReceiverLogic() {
	return receiverLogicNew;
}

FlowingFFTSpectre* Environment::getFlowingSpectre() {
	return flowingFFTSpectre;
}
