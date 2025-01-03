#pragma once

#include "Config.h"
#include "Device/DeviceController.h"
#include "FlowingFFTSpectre.h"
#include "SoundCardWriterThread.h"
#include "SignalModulatorThread.h"
#include "SoundCard.h"
#include "SoundProcessorThread.h"
#include "ViewModel.h"
#include "CircleBufferNew.h"
#include "Buffer.h"
#include "TXDataHandler.h"
#include "ComPortHandler.h"
#include "ALC.h"

class Environment {
	private:

		FFTData* fftData;

		Config* config = nullptr;
		DeviceController* deviceController = nullptr;

		//����� ��� �������� I Q
		//CircleBufferNew<uint8_t>* IQBufForSoundProc = nullptr;
		//CircleBufferNew<uint8_t>* IQBufForSpectre = nullptr;

		//CircleBuffer* IQSourceBuffer2 = nullptr;
		CircleBufferNew<float>* soundBuffer = nullptr;
		CircleBufferNew<float>* soundInputBuffer = nullptr;
		CircleBufferNew<float>* txBuffer = nullptr;

		SpectreHandler* specHandler = nullptr;

		SoundProcessorThread* soundProcessor = nullptr;

		SoundCardWriterThread* soundCardWriterThread = nullptr;

		SignalModulatorThread* soundCardInputReaderThread = nullptr;

		SoundCard* soundCard = nullptr;

		ViewModel* viewModel = nullptr;

		FlowingSpectre* flowingSpec = nullptr;

		ReceiverLogic* receiverLogic = nullptr;

		ComPortHandler* comPortHandler = nullptr;
	
		ALC* alc = nullptr;

	public:

		std::atomic_bool reloading = false;

		Environment();
		//void initReceivers();
		~Environment();
		void cleanUp();
		Config* getConfig();
		DeviceController* getDeviceController();
		void startProcessing();
		void stopProcessing();
		void makeReinit();
		void reload();
		SpectreHandler* getFFTSpectreHandler();
		SoundProcessorThread* getSoundProcessor();
		void init();
		SoundCard* getSoundCard();
		ViewModel* getViewModel();
		ReceiverLogic* getReceiverLogic();
		FlowingSpectre* getFlowingSpectre();
		SignalModulatorThread* getSoundCardInputReader();
		ComPortHandler* getComPortHandler();
		ALC* getALC();
};