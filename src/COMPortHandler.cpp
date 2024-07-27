#include "ComPortHandler.h"


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

		if (!serial.isDeviceOpen()) {
			bool result = connectToDevice();
			if (!result) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else {

			if (!deviceInited) {
				deviceInited = initDevice();
			}

			getStateTimerPoint2 = high_resolution_clock::now();
			if (duration_cast<chrono::milliseconds>(getStateTimerPoint2 - getStateTimerPoint1).count() > stateRequestTimePeriod) {
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

	auto portList = serial.getAvailablePorts();
	
	for (int i = 0; i < portList.size(); i++) {
		char errorOpening = serial.openDevice(portList[i].c_str(), 115200);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		
		if (errorOpening == 1) {
			//cout << "Port oppened: " << portList[i] << endl;
			string deviceAnswer = sendCMD(CMD_.INIT);

			//cout << "Answer: " << deviceAnswer << endl;

			if (deviceAnswer == CMD_.OK) {
				COM_PORT = portList[i];
				cout << "Device found!\n";
				return true;
			} else {
				serial.closeDevice();
			}
		}
	}

	return false;
}

string ComPortHandler::sendCMD(string cmd) {
	if (serial.isDeviceOpen()) {

		if (serial.writeString(string(cmd + CMD_.CMD_END).c_str()) == 1) {
			char str[128];

			int result = serial.readString(str, '\n', 127, 500);

			//cout << "Result: " << result << endl;

			string answer = "";

			if (result > 0) {
				answer = string(str, result);

				answer.pop_back();

				//cout << "CMD: " + cmd + "; answer: " + answer << endl;

				return answer;
			}
		}
	} else {
		cout << "Error sending cmd. Port is not open.\n";
	}
	
	return "";
}

ComPortHandler::ComPortHandler(Config* config) {
	this->config = config;

	currentDeviceState = config->myTranceiverDevice;
}

ComPortHandler::~ComPortHandler() {
	printf("~ComPortHandler()\r\n");
	if (serial.isDeviceOpen()) {
		if (COM_PORT.size() != 0) {
			if (deviceState.tx) sendCMD(CMD_.TX_ + "0");
			sendCMD(CMD_.CLOSE);
		}
		serial.closeDevice();
	}
}

std::thread ComPortHandler::start() {
	std::thread p(&ComPortHandler::run, this);
	return p;
}

bool ComPortHandler::isConnected() {
	bool result = false;

	if (serial.isDeviceOpen()) {
		if (COM_PORT.size() > 0) {
			result = true;
		}
	}

	return result;
}

void ComPortHandler::close() {
	if (serial.isDeviceOpen()) {
		sendCMD(CMD_.CLOSE);
		serial.closeDevice();
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
