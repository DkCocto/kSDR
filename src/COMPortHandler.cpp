#include "ComPortHandler.h"
#include <chrono>


void ComPortHandler::run() {
	using std::chrono::high_resolution_clock;

	chrono::time_point getStateTimerPoint1 = high_resolution_clock::now();
	chrono::time_point getStateTimerPoint2 = high_resolution_clock::now();

	while (true) {

		if (!config->WORKING) {
			printf("ComPortHandler Stopped\r\n");
			isWorking_ = false;
			return;
		}

		if (port != nullptr && !port->isOpen()) {
			bool result = connectToDevice();
			if (!result) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else {

			if (!deviceInited) {
				deviceInited = initDevice();
			}

			getStateTimerPoint2 = high_resolution_clock::now();
			if (duration_cast<chrono::milliseconds>(getStateTimerPoint2 - getStateTimerPoint1).count() > 20) {
				string answer = sendCMD(CMD_.GET_STATE);
				deviceState.parseState(answer);
				getStateTimerPoint1 = high_resolution_clock::now();
			}

			if (freq != (int)config->lastSelectedFreq) {

				string answer = sendCMD(CMD_.SET_FREQUENCY + to_string((int)config->lastSelectedFreq));
				if (answer == CMD_.OK || answer == CMD_.ERR) freq = (int)config->lastSelectedFreq;

			} if (currentDeviceState.att != config->myTranceiverDevice.att) {

				string answer = sendCMD(CMD_.ATT + to_string((int)config->myTranceiverDevice.att));
				if (answer == CMD_.OK) currentDeviceState.att = config->myTranceiverDevice.att;

			} if (currentDeviceState.pre != config->myTranceiverDevice.pre) {

				string answer = sendCMD(CMD_.PRE + to_string((int)config->myTranceiverDevice.pre));
				if (answer == CMD_.OK) currentDeviceState.pre = config->myTranceiverDevice.pre;

			} if (currentDeviceState.bypass != config->myTranceiverDevice.bypass) {

				string answer = sendCMD(CMD_.BYPASS + to_string((int)config->myTranceiverDevice.bypass));
				if (answer == CMD_.OK) currentDeviceState.bypass = config->myTranceiverDevice.bypass;

			} if (needToTxStart) {
				txStarted = sendCMD(CMD_.TX_ + "1") == CMD_.START_TX_OK;
				if (!txStarted) {
					readyToTxStart = true;
				} else {
					readyToTxStop = true;
					onStartStopTxListener->onStartTX();
				}
				needToTxStart = false;
			} if (needToTxStop) {
				txStarted = sendCMD(CMD_.TX_ + "0") != CMD_.STOP_TX_OK;
				if (txStarted) {
					readyToTxStop = true;
				} else {
					readyToTxStart = true;
					onStartStopTxListener->onStopTX();
				}
				needToTxStop = false;
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			}
		}

	}
}

bool ComPortHandler::initDevice() {
	bool result = false;
	result |= sendCMD(CMD_.ATT + to_string((int)config->myTranceiverDevice.att)).size() > 0;
	result |= sendCMD(CMD_.PRE + to_string((int)config->myTranceiverDevice.pre)).size() > 0;
	result |= sendCMD(CMD_.BYPASS + to_string((int)config->myTranceiverDevice.bypass)).size() > 0;
	return result;
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

			string deviceAnswer = sendCMD(CMD_.INIT);

			//cout << device.port + " PIPIKA: " + to_string(port->available()) + " " + deviceAnswer + "\n";

			if (deviceAnswer == CMD_.OK) {
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

string ComPortHandler::sendCMD(string cmd) {
	try {
		if (port != nullptr && port->isOpen()) {
			port->write(cmd + CMD_.CMD_END);
			string answer = port->readline();
			answer.pop_back();

			//cout << "CMD: " + cmd + "; answer: " + answer + "\n";

			return answer;
		}
	} catch (const std::exception& ex) {
		port->close();
	}
	return "";
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

void ComPortHandler::close() {
	if (port != nullptr) {
		if (port->isOpen()) {
			sendCMD(CMD_.CLOSE);
			port->close();
		}
	}
}

void ComPortHandler::startTX() {
	if (readyToTxStart) {
		readyToTxStart = false;
		needToTxStart = true;
	}
}

void ComPortHandler::stopTX() {
	if (readyToTxStop) {
		readyToTxStop = false;
		needToTxStop = true;
	}
}

bool ComPortHandler::isTXStarted() {
	return txStarted;
}

void ComPortHandler::setOnStartStopTxListener(OnStartStopTxListener* onStartStopTxListener) {
	this->onStartStopTxListener = onStartStopTxListener;
}

OnStartStopTxListener* ComPortHandler::getOnStartStopTxListener() {
	return this->onStartStopTxListener;
}

/*void ComPortHandler::setOnStopTxListener(OnStopTxListener* onStopTxListener) {
	this->onStopTxListener = onStopTxListener;
}*/

ComPortHandler::DeviceState ComPortHandler::getDeviceState() {
	return deviceState;
}
