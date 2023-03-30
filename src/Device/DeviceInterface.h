#pragma once

#include "DeviceN.h"

class DeviceInterface {

	protected:
		DeviceN* device;

	public:
		DeviceInterface(DeviceN* device);
		virtual void sendParamsToDevice() = 0;
};