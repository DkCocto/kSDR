#pragma once

#include "../Env.h"
#include "../Config.h"
#include "../CircleBuffer.h"
#include "HackRFInterface.h"
#include "RTLInterface.h"
#include "RSPInterface.h"
#include "SoundCardInterface.h"
#include "../SoundCard.h"

class DeviceController {
	private:
		Config* config = nullptr;

		DeviceN* device = nullptr;

		Result result;

		template<typename DEVICE, typename INTERFACE> void createDevice(DeviceType type);

		void resetResult();

		DeviceInterface* deviceInterface = nullptr;

		const bool DEBUG = true;

		CircleBufferNew<float>* txBuffer;

	public:

		void destroyDevice();

		void setResultToCreatedNotInit();

		DeviceController(Config* config);
		~DeviceController();

		DeviceType getCurrentDeviceType();

		DeviceN* getDevice();
		
		bool forceStop();

		void start(DeviceType deviceType);

		Result* getResult();

		DeviceInterface* getDeviceInterface();

		bool isStatusInitOk();
		bool isStatusInitFail();

		void setTXBuffer(CircleBufferNew<float>* txBuffer);

};