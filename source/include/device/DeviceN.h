#pragma once

#include <string>
#include "../Config.h"

enum Status {
	NOT_CREATED,
	CREATED_BUT_NOT_INIT,
	INIT_OK,
	INIT_FAULT
};

struct Result {
	Status status = NOT_CREATED;
	std::string err;
};

class DeviceN {
	public:

		Config* config;

		DeviceN (Config* config) {
			this->config = config;
		};
	
		virtual ~DeviceN() {};

		DeviceType deviceType = HACKRF;

		virtual Result start() = 0;
		virtual void stop() = 0;
};