#pragma once

#include "../Env.h"
#include "../Config.h"
#include "../CircleBuffer.h"
#include "HackRFInterface.h"
#include "RTLInterface.h"
#include "RSPInterface.h"

class DeviceController {
	private:
		Config* config = nullptr;

		DeviceN* device = nullptr;

		Result result;

		template<typename DEVICE, typename INTERFACE> void createDevice(DeviceType type);

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