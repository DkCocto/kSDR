#pragma once

#include "../Env.h"
#include "../Config.h"
#include "../CircleBuffer.h"
#include "HackRFDevice.h"
#include "HackRFInterface.h"

class DeviceController {
	private:
		Config* config = nullptr;
		DeviceN* device = nullptr;

		DeviceN::Result result;

		void createHackRFDevice();
		//void initRSPApi2Device();
		//void initRSPApi3Device();
		//void initRTLDevice();

		void resetResult();

		HackRfInterface* deviceInterface = nullptr;

		void destroy();

		std::vector<DataReceiver*> receivers;

		void resetReceivers();

	public:

		std::vector<DataReceiver*>* getReceivers();

		DeviceController(Config* config);
		~DeviceController();

		void addReceiver(DataReceiver* dataRceiver);
		DeviceType getCurrentDeviceType();

		DeviceN* getDevice();
		
		bool forceStop();

		void start(DeviceType deviceType);

		DeviceN::Result* getResult();

		HackRfInterface* getHackRfInterface();

		bool isReadyToReceiveCmd();
};