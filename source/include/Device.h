#pragma once

#include "string"

class Device {

	public:
		struct STATUS {
			bool isOK = false;
			std::string err;
			bool isInitProcessOccured = false;
		};
		Device();
		virtual ~Device(); 
		STATUS* status;	
};