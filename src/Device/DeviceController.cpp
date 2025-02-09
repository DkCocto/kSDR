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
            createDevice<HackRFDevice, HackRfInterface>(HACKRF);
            break;
        case RTL:
            createDevice<RTLDevice, RTLInterface>(RTL);
            break;
        case RSP:
            createDevice<RSPDevice, RSPInterface>(RSP);
            /*if (config->rsp.api == 3) {
                config->device = new RSPv2(config, iqSignalsCircleBuffer);
                break;
            }
            config->device = new RSP1(config, iqSignalsCircleBuffer);*/
            break;
        case SOUNDCARD:
            createDevice<SoundCardDevice, SoundCardInterface>(SOUNDCARD);
            break;
        default:
            createDevice<HackRFDevice, HackRfInterface>(HACKRF);
    }
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
    return result.status == INIT_FAULT;
}

void DeviceController::setTXBuffer(CircleBufferNew<float>* txBuffer) {
    this->txBuffer = txBuffer;
}

template<typename DEVICE, typename INTERFACE> void DeviceController::createDevice(DeviceType type) {
    device = new DEVICE(config);
    deviceInterface = new INTERFACE((DEVICE*)device);
    if (type == HACKRF) ((HackRfInterface*)deviceInterface)->setTXBuffer(txBuffer);
    config->deviceType = type;
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

void DeviceController::resetResult() {
    result = Result { NOT_CREATED, "" };
}


void DeviceController::setResultToCreatedNotInit() {
    result = Result { CREATED_BUT_NOT_INIT, "" };
}

DeviceController::DeviceController(Config* config) {
    this->config = config;
}

DeviceController::~DeviceController() {
    delete deviceInterface;
    delete device;
}

