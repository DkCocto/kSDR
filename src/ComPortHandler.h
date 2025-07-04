#pragma once

#include "Config.h"
#include "Thread/MyThread.h"
#include "KalmanFilter.h"
#include <chrono>
#include "serialib.h"

using namespace std;

class OnStartStopTxListener {
public:
	virtual void onStartTX() = 0;
	virtual void onStopTX() = 0;
};

class ComPortHandler : public MyThread {

private:
	Config* config;

	void run();

	bool initDevice();

	serialib serial;

	string COM_PORT = "";
	const int PORT_SPEED = 115200;

	int freq = 0;

	atomic_bool needToTxStart = false;
	atomic_bool readyToTxStart = true;
	atomic_bool needToTxStop = false;
	atomic_bool readyToTxStop = false;
	bool txStarted = false;

	const int stateRequestTimePeriodMS = 1;

	struct CMD {
		const string INIT = "i";
		const string CLOSE = "c";

		const string PRE = "p";
		const string ATT = "a";
		const string BYPASS = "b";

		const string GET_STATE = "s";

		const string SET_FREQUENCY = "f";

		const string TX_ = "t";

		const string OK = "ok";
		const string ERR = "er";
		const string START_TX_OK = "start_tx_ok";
		const string STOP_TX_OK = "stop_tx_ok";

		const string CMD_END = "\n";
		const string CMD_DELIMITER = ",";
	} CMD_;

	class DeviceState {

	public:

		enum BAND {
			B_160,
			B_80,
			B_40,
			B_30,
			B_20,
			B_17,
			B_15,
			B_12,
			B_10,
			UNDEF
		};

		BAND band;
		long freq;
		bool tx;

		float volts;
		float current;

		bool amp;
		bool att;
		bool bypass;
		bool autoBypass;

		float swr;
		float power;
		float rawPower;

		KalmanFilter* kFVolts = new KalmanFilter(2, 0.5);
		KalmanFilter* kFCurrent = new KalmanFilter(2, 0.5);
		KalmanFilter* kFSwr = new KalmanFilter(2, 0.5);
		KalmanFilter* kFPower = new KalmanFilter(2, 0.5);
		
		DeviceState() {
			band = B_40;
			freq = 7100000;
			tx = false;
			volts = 0.0f;
			current = 0.0f;

			amp = false;
			att = false;
			bypass = false;
			autoBypass = false;

			swr = 0.0f;
			power = 0.0f;
		}

		bool parseState(string state) {
			vector<std::string> result;
			stringstream ss(state);
			string item;

			while (getline(ss, item, ';')) {
				result.push_back(item);
			}

			if (result.size() != 11) return false;

			band = BAND(atoi(result[0].c_str()));
			freq = atoi(result[1].c_str());
			tx = atoi(result[2].c_str());
			amp = atoi(result[3].c_str());
			att = atoi(result[4].c_str());
			bypass = atoi(result[5].c_str());
			autoBypass = atoi(result[6].c_str());

			swr = kFSwr->filter(atof(result[7].c_str()));
			rawPower = atof(result[8].c_str());
			power = kFPower->filter(rawPower);

			volts = kFVolts->filter(atof(result[9].c_str()));
			current = kFCurrent->filter(atof(result[10].c_str()));
		}
		bool isDevicePoweredON() {
			if (!DRY_TX) return volts >= 9;
			else return true;
		}
	} deviceState;

	bool deviceInited = false;

	Config::MyTranceiverDevice currentDeviceState;

	bool connectToDevice();

	string sendCMD(string cmd);

	OnStartStopTxListener* onStartStopTxListener = nullptr;

public:
	ComPortHandler(Config* config);
	~ComPortHandler();

	std::thread start();

	bool isConnected();
	void close();
	void startTX();
	void stopTX();
	bool isTXStarted();
	void setOnStartStopTxListener(OnStartStopTxListener* onStartStopTxListener);

	OnStartStopTxListener* getOnStartStopTxListener();

	DeviceState getDeviceState();
};