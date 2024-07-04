#pragma once

#include "serial/serial.h"
#include "Config.h"
#include "Thread/MyThread.h"

using namespace std;
using namespace serial;

class ComPortHandler : public MyThread {

private:
	Config* config;

	void run();

	bool initDevice();

	string COM_PORT = "";
	const int PORT_SPEED = 115200;

	Serial* port = nullptr;

	int freq = 0;

	struct CMD {
		const string INIT = "i";
		const string CLOSE = "c";

		const string PRE = "p";
		const string ATT = "a";
		const string BYPASS = "b";

		const string SET_FREQUENCY = "f";

		const string OK = "ok";
		const string ERR = "er";

		const string CMD_END = "\n";
		const string CMD_DELIMITER = ",";
	} CMD_;

	bool deviceInited = false;

	Config::MyTranceiverDevice currentDeviceState;

	bool connectToDevice();

	string sendCMD(string cmd);

public:
	ComPortHandler(Config* config);
	~ComPortHandler();

	std::thread start();

	bool isConnected();
	void close();
};