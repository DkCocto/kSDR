#pragma once

#include "Config.h"
#include "Device/DeviceController.h"

class Environment {
	Config* config = nullptr;
	DeviceController* deviceController = nullptr;

	//����� ��� �������� I Q
	CircleBuffer* IQSourceBuffer = nullptr;

	public:
		Environment();
		~Environment();
		Config* getConfig();
		DeviceController* getDeviceController();
		CircleBuffer* getIQSourceBuffer();
};