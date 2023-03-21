#include "DeviceN.h"

DeviceN::DeviceN(Config* config) {
	this->config = config;
}

void DeviceN::addReceiver(CircleBuffer* circleBuffer) {
	receivers.push_back(circleBuffer);
}

std::vector<CircleBuffer*>* DeviceN::getReceivers() {
	return &receivers;
}