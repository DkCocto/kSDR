#pragma once

#include "../Config.h"
#include "../CircleBuffer.h"
#include "vector"

class DeviceN {
	
	private:

		std::vector<CircleBuffer*>* receivers = nullptr;

	public:

		Config* config;

		DeviceN(Config* config);
		
		void setReceivers(std::vector<CircleBuffer*>* receivers);
		std::vector<CircleBuffer*>* getReceivers();

		virtual ~DeviceN() {};

		DeviceType deviceType = HACKRF;

		enum Status {
			NOT_CREATED,
			CREATED_BUT_NOT_INIT,
			INIT_OK,
			INIT_BUT_FAIL
		};

		struct Result {
			Status status = NOT_CREATED;
			std::string err;
		};

		virtual void setFreq(uint64_t frequency) = 0;
		virtual void setSampleRate(int sampleRate) = 0;
		virtual Result start() = 0;
		virtual void stop() = 0;

		CircleBuffer* getReceiver();
};