#include "DeviceController.h"

DeviceType DeviceController::getCurrentDeviceType() {
    return config->deviceType;
}

DeviceN* DeviceController::getDevice() {
    return device;
}

bool DeviceController::forceStop() {
    if (DEBUG) printf("Trying to force stop device...\r\n");
    if (device != nullptr) {
        destroy();
        return true;
    }
    if (DEBUG) printf("Stopping error. Device was null.\r\n");
    return false;
}

void DeviceController::destroy() {
    if (DEBUG) printf("Trying to destroy device...\r\n");
    if (device != nullptr) {
        delete deviceInterface;
        deviceInterface = nullptr;
        delete device;
        device = nullptr;
        resetResult();
        if (DEBUG) printf("Device destroyed!\r\n");
    }
}

void DeviceController::resetReceivers() {
    for (int i = 0; i < receivers.size(); i++) {
        receivers[i]->reset();
    }
}

void DeviceController::start(DeviceType deviceType) {
    if (DEBUG) printf("Starting device...\r\n");

    if (result.status == DeviceN::INIT_OK) {
        if (DEBUG) printf("Device already started! You need to stop device first!\r\n");
        return;
    }

    if (device != nullptr) {
        if (DEBUG) printf("You need to destroy device first!\r\n");
        return;
    }

    switch (deviceType) {
        case HACKRF:
            createHackRFDevice();
            break;
        case RTL:
            break;
        case RSP:
            /*if (config->rsp.api == 3) {
                config->device = new RSPv2(config, iqSignalsCircleBuffer);
                break;
            }
            config->device = new RSP1(config, iqSignalsCircleBuffer);*/
            break;
        default:
            createHackRFDevice();
    }
    resetReceivers();
}

DeviceN::Result* DeviceController::getResult() {
    return &result;
}

HackRfInterface* DeviceController::getHackRfInterface() {
    return deviceInterface;
}

bool DeviceController::isReadyToReceiveCmd() {
    return result.status == DeviceN::INIT_OK;
}

void DeviceController::createHackRFDevice() {
    device = new HackRFDevice(config);
    device->setReceivers(&receivers);
    deviceInterface = new HackRfInterface((HackRFDevice*)device);
    config->deviceType = HACKRF;
    result = DeviceN::Result { DeviceN::CREATED_BUT_NOT_INIT };
    result = device->start();

    if (DEBUG) {
        if (result.status == DeviceN::INIT_OK) {
            printf("Device started!\r\n");
        } else {
            printf("Error starting device!\r\n");
        }
    }
}

void DeviceController::resetResult() {
    result = DeviceN::Result { DeviceN::NOT_CREATED, "" };
}

DeviceController::DeviceController(Config* config) {
    this->config = config;
    //start(config->deviceType);
}

DeviceController::~DeviceController() {
    delete device;
    delete deviceInterface;
}

void DeviceController::addReceiver(CircleBuffer* circleBuffer) {
    receivers.push_back(circleBuffer);
}