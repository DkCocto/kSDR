#pragma once

class DataReceiver {
public:
	virtual void write(float value) = 0;
	virtual void reset() = 0;
};