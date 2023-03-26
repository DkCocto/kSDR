#include "DeviceN.h"

DeviceN::DeviceN(Config* config) {
	this->config = config;
}

void DeviceN::setReceivers(std::vector<DataReceiver*>* receivers) {
	this->receivers = receivers;
}

std::vector<DataReceiver*>* DeviceN::getReceivers() {
	return receivers;
}