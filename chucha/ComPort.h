#pragma once

#include "comport/rs232.h"
#include "thread"
#include "CircleBuffer.h"

class ComPort {

	CircleBuffer* circleBuffer;

public:
	ComPort(CircleBuffer* circleBuffer) {
		this->circleBuffer = circleBuffer;
	}
	void read();
	std::thread start();
};