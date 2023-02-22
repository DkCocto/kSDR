#pragma once

#include "string"

class Device {
	public:
		struct STATUS {
			bool OK = false;
			std::string err;
			bool initDone = false;
		};
		virtual ~Device(); 
		STATUS* status;	
};