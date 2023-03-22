#include "DeviceN.h"

DeviceN::DeviceN(Config* config) {
	this->config = config;
}

void DeviceN::setReceivers(std::vector<CircleBuffer*>* receivers) {
	this->receivers = receivers;
}

std::vector<CircleBuffer*>* DeviceN::getReceivers() {
	return receivers;
}