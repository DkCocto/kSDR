#include "Environment.h"

Environment::Environment() {
	config = new Config();
	viewModel = new ViewModel(config);											//is not need to recreate
	deviceController = new DeviceController(config);							//is not need to recreate
	
	//init buffers
	//IQBufForSoundProc = new CircleBufferNew(config->circleBufferLen);			//is not need to recreate
	//IQBufForSpectre = new CircleBufferNew<uint8_t>(config->circleBufferLen);
	
	//IQSourceBuffer2 = new CircleBuffer(config->circleBufferLen);				//is not need to recreate
	soundBuffer = new CircleBufferNew<float>(config->circleBufferLen);			//is not need to recreate
	
	fftData = new FFTData(config->fftLen / 2);									//is not need to recreate
	specHandler = new SpectreHandler(config, fftData, viewModel, deviceController);	//need to recreate
	flowingSpec = new FlowingSpectre(config, viewModel);						//is not need to recreate
	receiverLogic = new ReceiverLogic(config, viewModel, flowingSpec);			//need to setup new flowingSpec during its recreating

	//initReceivers();
}

/*void Environment::initReceivers() {
	//deviceController->addReceiver(IQSourceBuffer);						//is not need to recreate
	deviceController->addReceiver(IQBufForSoundProc);							//is not need to recreate
	deviceController->addReceiver(IQBufForSpectre);						//is not need to recreate
}*/

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
	//delete IQBufForSoundProc;
}

Config* Environment::getConfig() {
	return config;
}

DeviceController* Environment::getDeviceController() {
	return deviceController;
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
	specHandler = new SpectreHandler(config, fftData, viewModel, deviceController);

	//deviceController->getReceivers()->clear();
	//initReceivers();

	flowingSpec->setPos(
		flowingSpec->getSpectrePosByAbsoluteFreq((double)config->spectre.visibleStartFreq),
		flowingSpec->getSpectrePosByAbsoluteFreq((double)config->spectre.visibleStopFreq)
	);

	receiverLogic->setFreq(config->lastSelectedFreq);

	startProcessing();
	reloading = false;
}

void Environment::startProcessing() {
	if (soundProcessor != nullptr) {
		if (soundProcessor->isWorking()) {
			printf("Need to stop processing first!\r\n");
			return;
		}
	}

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

void Environment::init() {
	soundCard = new SoundCard(config);
	soundProcessor = new SoundProcessorThread(deviceController, viewModel, receiverLogic, config, soundBuffer, specHandler);
	circleBufferWriterThread = new CircleBufferWriterThread(config, deviceController, soundBuffer, soundCard);
}

void Environment::stopProcessing() {
	//Stop 3 threads: sound process, soundcard writer, fft handler
	config->WORKING = false; 

	while(soundProcessor->isWorking() || circleBufferWriterThread->isWorking() || specHandler->isWorking());

	delete soundProcessor;
	soundProcessor = nullptr;
	
	delete circleBufferWriterThread;
	circleBufferWriterThread = nullptr;
	
	delete soundCard;
	soundCard = nullptr;

	deviceController->forceStop();
}

SpectreHandler* Environment::getFFTSpectreHandler() {
	return specHandler;
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
