#pragma once

#include "DeviceInterface.h"
#include "SoundCardDevice.h"

class SoundCardInterface : public DeviceInterface {
	SoundCardDevice* soundCardDevice;

public:

	SoundCardInterface(SoundCardDevice* soundCardDevice) : DeviceInterface(soundCardDevice) {
		this->soundCardDevice = soundCardDevice;
	}

	void sendParamsToDevice() {

	}

};