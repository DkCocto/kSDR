#include "Environment.h"

Environment::Environment() {
	config = new Config();
	deviceController = new DeviceController(config);					//is not need to recreate
	IQSourceBuffer = new CircleBuffer(config->circleBufferLen);			//is not need to recreate
	deviceController->addReceiver(IQSourceBuffer);						//is not need to recreate
	soundBuffer = new CircleBuffer(config->circleBufferLen);			//is not need to recreate
	fftData = new FFTData(config->fftLen / 2);							//is not need to recreate
	specHandler = new SpectreHandler(config, fftData);					//need to recreate
	viewModel = new ViewModel(config);									//is not need to recreate
	flowingSpec = new FlowingSpectre(config, viewModel);				//is not need to recreate
	receiverLogic = new ReceiverLogic(config, viewModel, flowingSpec);	//need to setup new flowingSpec during its recreating
}

Environment::~Environment() {
	cleanUp();
}

void Environment::cleanUp() {
	stopProcessing();
	delete viewModel;
	delete flowingSpec;
	delete config;
	delete specHandler;
	delete deviceController;
	delete soundBuffer;
	delete receiverLogic;
	delete IQSourceBuffer;
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
	soundProcessor = new SoundProcessorThread(deviceController, viewModel, receiverLogic, config, IQSourceBuffer, soundBuffer, specHandler);
	circleBufferWriterThread = new CircleBufferWriterThread(config, deviceController, soundBuffer, soundCard);
}

void Environment::makeReload() {
	if (soundProcessor != nullptr) {
		printf("Need to stop processing first!\r\n");
		reloading = false;
		return;
	}
	reloading = true;
}

void Environment::reload() {
	viewModel->storeToConfig();
	config->save();
	config->prepareConfiguration();
	viewModel->loadFromConfig();

	fftData->init(config->fftLen / 2);

	delete specHandler;
	specHandler = nullptr;
	specHandler = new SpectreHandler(config, fftData);

	flowingSpec->setPos(
		flowingSpec->getSpectrePosByAbsoluteFreq((double)config->spectre.visibleStartFreq),
		flowingSpec->getSpectrePosByAbsoluteFreq((double)config->spectre.visibleStopFreq)
	);

	receiverLogic->setFreq(config->lastSelectedFreq);

	startProcessing();
	reloading = false;
}

void Environment::startProcessing() {
	deviceController->start(config->deviceType);

	if (deviceController->isReadyToReceiveCmd()) {
		config->WORKING = true;
		
		init();

		specHandler->start().detach();

		//Инициализируем звуковую карту
		soundCard->open();

		circleBufferWriterThread->start().detach();
		soundProcessor->start().detach();

		//while (!soundProcessor->isWorking() || !circleBufferWriterThread->isWorking() || !specHandler->isWorking());
		
	}
}

void Environment::stopProcessing() {
	deviceController->forceStop();

	//Stop 3 threads: sound process, soundcard writer, fft handler
	config->WORKING = false; 

	while(soundProcessor->isWorking() || circleBufferWriterThread->isWorking() || specHandler->isWorking());

	delete soundProcessor;
	soundProcessor = nullptr;
	
	delete circleBufferWriterThread;
	circleBufferWriterThread = nullptr;
	
	delete soundCard;
	soundCard = nullptr;
}

SpectreHandler* Environment::getFFTSpectreHandler() {
	return specHandler;
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
	return receiverLogic;
}

FlowingSpectre* Environment::getFlowingSpectre() {
	return flowingSpec;
}
