#include "Environment.h"

Environment::Environment() {
	config = new Config();
	viewModel = new ViewModel(config);											
	deviceController = new DeviceController(config);							
	
	soundBuffer = new CircleBufferNew<float>(config);							

	soundInputBuffer = new CircleBufferNew<float>(config);							
	
	deviceController->setTXBuffer(soundInputBuffer);

	fftData = new FFTData(config->fftLen / 2);									
	specHandler = new SpectreHandler(config, fftData, viewModel, deviceController);	//need to recreate
	flowingSpec = new FlowingSpectre(config, viewModel);						
	receiverLogic = new ReceiverLogic(config, viewModel, flowingSpec);				//need to setup new flowingSpec during its recreating
	alc = new ALC(config);
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
	delete soundInputBuffer;
	delete receiverLogic;
	delete comPortHandler;
	delete alc;
}

Config* Environment::getConfig() {
	return config;
}

DeviceController* Environment::getDeviceController() {
	return deviceController;
}

void Environment::makeReinit() {
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

	if (deviceController->isStatusInitOk()) {
		config->WORKING = true;
		
		init();

		specHandler->start().detach();

		//�������������� �������� �����
		soundCard->open();
		if (config->deviceType == DeviceType::SOUNDCARD) {
			auto device = (SoundCardDevice*)deviceController->getDevice();
			device->setInputStream(soundCard->getInputStream());
			soundCard->startInput();
		}

		soundCardWriterThread->start().detach();

		soundCardInputReaderThread->start().detach();

		soundProcessor->start().detach();

		if (config->deviceType == HACKRF) {
			comPortHandler->start().detach();
		}
	}
}

void Environment::init() {
	soundCard = new SoundCard(config);
	soundProcessor = new SoundProcessorThread(deviceController, viewModel, receiverLogic, config, soundBuffer, specHandler);
	soundCardWriterThread = new SoundCardWriterThread(config, deviceController, soundBuffer, soundCard);
	soundCardInputReaderThread = new SignalModulatorThread(config, soundInputBuffer, soundCard);
	comPortHandler = new ComPortHandler(config);
}

void Environment::stopProcessing() {
	//Stop 3 threads: sound process, soundcard writer, fft handler
	config->WORKING = false; 

	if (soundProcessor != nullptr) {
		while (soundProcessor->isWorking());
		delete soundProcessor;
		soundProcessor = nullptr;
	}

	if (soundCardWriterThread != nullptr) {
		while (soundCardWriterThread->isWorking());
		delete soundCardWriterThread;
		soundCardWriterThread = nullptr;
	}

	if (soundCardInputReaderThread != nullptr) {
		while (soundCardInputReaderThread->isWorking());
		delete soundCardInputReaderThread;
		soundCardInputReaderThread = nullptr;
	}

	if (comPortHandler != nullptr) {
		while (comPortHandler->isWorking());
		delete comPortHandler;
		comPortHandler = nullptr;
	}

	if (specHandler != nullptr) {
		while (specHandler->isWorking());
	}

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

SignalModulatorThread* Environment::getSoundCardInputReader() {
	return soundCardInputReaderThread;
}

ComPortHandler* Environment::getComPortHandler() {
	return comPortHandler;
}

ALC* Environment::getALC() {
	return alc;
}
