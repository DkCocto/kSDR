#pragma once

#include "Config.h"
#include "Device/DeviceController.h"
#include "FlowingFFTSpectre.h"
#include "CircleBufferWriterThread.h"
#include "SoundCard.h"
#include "SoundProcessorThread.h"
#include "ViewModel.h"

class Environment {
	private:

		FFTData* fftData;

		Config* config = nullptr;
		DeviceController* deviceController = nullptr;

		//Буфер для сигналов I Q
		CircleBuffer* IQSourceBuffer = nullptr;

		CircleBuffer* soundBuffer = nullptr;

		FFTSpectreHandler* fftSpectreHandler = nullptr;

		SoundProcessorThread* soundProcessor = nullptr;

		CircleBufferWriterThread* circleBufferWriterThread = nullptr;

		SoundCard* soundCard = nullptr;

		ViewModel* viewModel = nullptr;

		FlowingFFTSpectre* flowingFFTSpectre = nullptr;

		ReceiverLogic* receiverLogicNew = nullptr;

	public:
		Environment();
		~Environment();
		Config* getConfig();
		DeviceController* getDeviceController();
		CircleBuffer* getIQSourceBuffer();
		void startProcessing();
		void stopProcessing();
		FFTSpectreHandler* getFFTSpectreHandler();
		CircleBuffer* getSoundBuffer();
		SoundProcessorThread* getSoundProcessor();
		void init();
		SoundCard* getSoundCard();
		ViewModel* getViewModel();
		ReceiverLogic* getReceiverLogic();
		FlowingFFTSpectre* getFlowingSpectre();
};