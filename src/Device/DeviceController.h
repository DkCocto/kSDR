#pragma once

#include "../Env.h"
#include "../Config.h"
#include "../CircleBuffer.h"
#include "HackRFDevice.h"
#include "HackRFInterface.h"
#include "RTLDevice.h"
#include "RTLInterface.h"

class DeviceController {
	private:
		Config* config = nullptr;

		DeviceN* device = nullptr;

		Result result;

		void createHackRFDevice();
		//void initRSPApi2Device();
		//void initRSPApi3Device();
		void createRTLDevice();

		void resetResult();

		DeviceInterface* deviceInterface = nullptr;

		//std::vector<DataReceiver*> receivers;

		//void resetReceivers();
		const bool DEBUG = true;

	public:

		void destroyDevice();

		//std::vector<DataReceiver*>* getReceivers();

		void setResultToCreatedNotInit();

		DeviceController(Config* config);
		~DeviceController();

		//void addReceiver(DataReceiver* dataRceiver);
		DeviceType getCurrentDeviceType();

		DeviceN* getDevice();
		
		bool forceStop();

		void start(DeviceType deviceType);

		Result* getResult();

		DeviceInterface* getDeviceInterface();

		bool isStatusInitOk();
		bool isStatusInitFail();
};