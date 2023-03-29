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
        destroyDevice();
        return true;
    }
    if (DEBUG) printf("Stopping error. Device was null.\r\n");
    return false;
}

void DeviceController::destroyDevice() {
    if (DEBUG) printf("Trying to destroyDevice device...\r\n");
    if (device != nullptr) {
        delete deviceInterface;
        deviceInterface = nullptr;
        delete device;
        device = nullptr;
        resetResult();
        if (DEBUG) printf("Device destroyed!\r\n");
    }
}

void DeviceController::start(DeviceType deviceType) {
    if (DEBUG) printf("Starting device...\r\n");

    if (result.status == INIT_OK) {
        if (DEBUG) printf("Device already started! You need to stop device first!\r\n");
        return;
    }

    if (device != nullptr) {
        if (DEBUG) printf("You need to destroyDevice device first!\r\n");
        return;
    }

    switch (deviceType) {
        case HACKRF:
            createHackRFDevice();
            break;
        case RTL:
            createRTLDevice();
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
    //resetReceivers();
}

Result* DeviceController::getResult() {
    return &result;
}

DeviceInterface* DeviceController::getDeviceInterface() {
    return deviceInterface;
}

bool DeviceController::isStatusInitOk() {
    return result.status == INIT_OK;
}

bool DeviceController::isStatusInitFail() {
    return result.status == INIT_BUT_FAIL;
}

void DeviceController::createHackRFDevice() {
    device = new HackRFDevice(config);
    //device->setReceivers(&receivers);
    deviceInterface = new HackRfInterface((HackRFDevice*)device);
    config->deviceType = HACKRF;
    result = Result { CREATED_BUT_NOT_INIT };
    result = device->start();

    if (DEBUG) {
        if (result.status == INIT_OK) {
            printf("Device started!\r\n");
        } else {
            printf("Error starting device!\r\n");
        }
    }
}

void DeviceController::createRTLDevice() {
    device = new RTLDevice(config);
    //device->setReceivers(&receivers);
    deviceInterface = new RTLInterface((RTLDevice*)device);
    config->deviceType = RTL;
    result = Result{ CREATED_BUT_NOT_INIT };
    result = device->start();

    if (DEBUG) {
        if (result.status == INIT_OK) {
            printf("Device started!\r\n");
        }
        else {
            printf("Error starting device!\r\n");
        }
    }
}

void DeviceController::resetResult() {
    result = Result { NOT_CREATED, "" };
}


void DeviceController::setResultToCreatedNotInit() {
    result = Result{ CREATED_BUT_NOT_INIT, "" };
}

DeviceController::DeviceController(Config* config) {
    this->config = config;
}

DeviceController::~DeviceController() {
    delete deviceInterface;
    delete device;
}