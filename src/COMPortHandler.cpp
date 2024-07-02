#include "ComPortHandler.h"

void ComPortHandler::run() {
	while (true) {
		if (!config->WORKING) {
			printf("ComPortHandler Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (port != nullptr && !port->isOpen()) {
			connectToDevice();
		} else {
			if (freq != (int)config->lastSelectedFreq) {

				freq = (int)config->lastSelectedFreq;
				sendCMD(CMD_.SET_FREQUENCY + to_string(freq));

			} if (currentDeviceState.att != config->myTranceiverDevice.att) {

				sendCMD(CMD_.ATT + to_string((int)config->myTranceiverDevice.att));
				currentDeviceState.att = config->myTranceiverDevice.att;

			} if (currentDeviceState.pre != config->myTranceiverDevice.pre) {

				sendCMD(CMD_.PRE + to_string((int)config->myTranceiverDevice.pre));
				currentDeviceState.pre = config->myTranceiverDevice.pre;

			} else {

				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			}
		}

	}
}

bool ComPortHandler::connectToDevice() {

	COM_PORT = "";

	std::vector<serial::PortInfo> devices_found = serial::list_ports();

	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	while (iter != devices_found.end()) {
		serial::PortInfo device = *iter++;
		//printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
		port->setPort(device.port);
		port->open();
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		if (port->isOpen()) {

			sendCMD(CMD_.INIT);

			string deviceAnswer = port->readline();

			//cout << device.port + " PIPIKA: " + to_string(port->available()) + " " + deviceAnswer + "\n";

			if (deviceAnswer == CMD_.OK + CMD_.CMD_END) {
				COM_PORT = device.port;
				cout << "Com port device has been successfully connected!\n";
				return true;
			} else {
				port->close();
			}
		}
	}

	return false;
}

void ComPortHandler::sendCMD(string cmd) {
	if (port != nullptr && port->isOpen()) {
		port->write(cmd + CMD_.CMD_END);
	}
}

ComPortHandler::ComPortHandler(Config* config) {
	this->config = config;

	port = new Serial("", PORT_SPEED, serial::Timeout::simpleTimeout(500));

	currentDeviceState = config->myTranceiverDevice;
}

ComPortHandler::~ComPortHandler() {
	printf("~ComPortHandler()\r\n");
	if (port != nullptr) {
		if (port->isOpen()) {
			sendCMD(CMD_.CLOSE);
			port->close();
		}
		delete port;
	}
}

std::thread ComPortHandler::start() {
	std::thread p(&ComPortHandler::run, this);
	return p;
}

bool ComPortHandler::isConnected() {
	bool result = false;

	if (port != nullptr) {
		if (port->isOpen()) {
			if (COM_PORT.size() > 0) {
				result = true;
			}
		}
	}

	return result;
}
