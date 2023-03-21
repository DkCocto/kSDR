#include "Environment.h"

Environment::Environment() {
	config = new Config();
	deviceController = new DeviceController(config);
	IQSourceBuffer = new CircleBuffer(config->circleBufferLen);
	deviceController->addReceiver(IQSourceBuffer);
}

Environment::~Environment() {
	delete IQSourceBuffer;
	delete config;
	delete deviceController;
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