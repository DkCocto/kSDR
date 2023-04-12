#include "RSPDevice.h"

RSPDevice::~RSPDevice() {
	printf("~RSPDevice()\r\n");
	stop();
	delete bufferForSpec;
	delete bufferForProc;
}

Result RSPDevice::start() {
    unsigned int ndev;
    int i;
    float ver = 0.0;
    sdrplay_api_ErrT err;
    sdrplay_api_RxChannelParamsT* chParams;

    unsigned int chosenIdx = 0;

    initResult = { CREATED_BUT_NOT_INIT, std::string("") };

    if ((err = sdrplay_api_Open()) != sdrplay_api_Success) {
        initResult.err.append("sdrplay_api_Open failed: ");
        initResult.err.append(sdrplay_api_GetErrorString(err));
        initResult.status = INIT_FAULT;
        return initResult;
    } else {

        if ((err = sdrplay_api_DebugEnable(NULL, (sdrplay_api_DbgLvl_t)1)) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_DebugEnable failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            return initResult;
        }

        if ((err = sdrplay_api_ApiVersion(&ver)) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_ApiVersion failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            return initResult;
        }

        if (ver != SDRPLAY_API_VERSION) {
            initResult.err.append("API version don't match");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            sdrplay_api_Close();
            return initResult;
        }

        sdrplay_api_LockDeviceApi();

        // Fetch list of available devices
        if ((err = sdrplay_api_GetDevices(devs, &ndev, sizeof(devs) / sizeof(sdrplay_api_DeviceT))) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_GetDevices failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            sdrplay_api_UnlockDeviceApi();
            sdrplay_api_Close();
            return initResult;
        }
        printf("MaxDevs=%d NumDevs=%d\n", sizeof(devs) / sizeof(sdrplay_api_DeviceT), ndev);

        if (ndev > 0) {
            for (i = 0; i < (int)ndev; i++) {
                if (devs[i].hwVer == SDRPLAY_RSPduo_ID)
                    printf("Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x rspDuoMode=0x%.2x\n", i, devs[i].SerNo, devs[i].hwVer, devs[i].tuner, devs[i].rspDuoMode);
                else
                    printf("Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x\n", i, devs[i].SerNo, devs[i].hwVer, devs[i].tuner);
            }
        }

        // Pick first device of any type
        for (i = 0; i < (int)ndev; i++) {
            chosenIdx = i;
            break;
        }

        printf("chosenDevice = %d\n", chosenIdx);
        chosenDevice = &devs[chosenIdx];

        // Select chosen device
        if ((err = sdrplay_api_SelectDevice(chosenDevice)) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_SelectDevice failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            sdrplay_api_UnlockDeviceApi();
            sdrplay_api_Close();
            return initResult;
        }

        sdrplay_api_UnlockDeviceApi();

        // Retrieve device parameters so they can be changed if wanted
        if ((err = sdrplay_api_GetDeviceParams(chosenDevice->dev, &deviceParams)) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_GetDeviceParams failed failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;
            sdrplay_api_Close();
            return initResult;
        }

        // Check for NULL pointers before changing settings
        if (deviceParams == NULL) {
            initResult.err.append("sdrplay_api_GetDeviceParams returned NULL deviceParams pointer");
            initResult.status = INIT_FAULT;
            sdrplay_api_Close();
            return initResult;
        }

        deviceParams->devParams->fsFreq.fsHz = config->rsp.deviceSamplingRate;
        deviceParams->rxChannelA->ctrlParams.decimation = sdrplay_api_DecimationT{ 1, (unsigned char)config->rsp.deviceDecimationFactor, 0 };

        // Configure tuner parameters (depends on selected Tuner which set of parameters to use)
        chParams = (chosenDevice->tuner == sdrplay_api_Tuner_B) ? deviceParams->rxChannelB : deviceParams->rxChannelA;
        if (chParams != NULL) {
            chParams->tunerParams.rfFreq.rfHz = config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);
            chParams->tunerParams.bwType = (sdrplay_api_Bw_MHzT)config->rsp.basebandFilter;

            chParams->tunerParams.ifType = sdrplay_api_IF_Zero;

            chParams->tunerParams.gain.gRdB = config->rsp.gain;
            chParams->tunerParams.gain.LNAstate = config->rsp.lna;

            // Disable AGC
            chParams->ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
        }
        else {
            initResult.err.append("sdrplay_api_GetDeviceParams returned NULL chParams pointer");
            initResult.status = INIT_FAULT;
            sdrplay_api_Close();
            return initResult;
        }

        cbFns.StreamACbFn = RSPDevice::StreamACallback;
        cbFns.StreamBCbFn = RSPDevice::StreamBCallback;
        cbFns.EventCbFn = RSPDevice::EventCallback;

        if ((err = sdrplay_api_Init(chosenDevice->dev, &cbFns, (void*)this)) != sdrplay_api_Success) {
            initResult.err.append("sdrplay_api_Init failed: ");
            initResult.err.append(sdrplay_api_GetErrorString(err));
            initResult.status = INIT_FAULT;         
            sdrplay_api_Close();
            return initResult;
        }
    }

    initResult.status = INIT_OK;
    return initResult;
}

void RSPDevice::stop() {
    if (initResult.status == INIT_OK) {
        sdrplay_api_Uninit(chosenDevice->dev);
        sdrplay_api_ReleaseDevice(chosenDevice);
    }
    sdrplay_api_UnlockDeviceApi();
    sdrplay_api_Close();
}

CircleBufferNew<short>* RSPDevice::getBufferForSpec() {
    return bufferForSpec;
}

CircleBufferNew<short>* RSPDevice::getBufferForProc() {
    return bufferForProc;
}

sdrplay_api_DeviceT* RSPDevice::getChosenDev() {
    return chosenDevice;
}

void RSPDevice::StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    RSPDevice* rspDevice = (RSPDevice*)cbContext;

    if (reset) {
        printf("sdrplay_api_StreamACallback: numSamples=%d\n", numSamples);
    }

    short* buf = new short[numSamples * 2];

    for (int i = 0; i < numSamples; i++) {
        buf[2 * i] = xi[i];
        buf[2 * i + 1] = xq[i];
    }

    rspDevice->getBufferForProc()->write(buf, numSamples * 2);
    rspDevice->getBufferForSpec()->write(buf, numSamples * 2);

    delete[] buf;

    if (rspDevice->isNeedToSendFreq()) rspDevice->sendFreqToDevice();
    if (rspDevice->isNeedToSendGain()) rspDevice->sendGainToDevice();
    if (rspDevice->isNeedToSendFilter()) rspDevice->sendBasebandFilterToDevice();
}

void RSPDevice::StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    if (reset) printf("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);
}

void RSPDevice::EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext) {
    RSPDevice* rspDevice = (RSPDevice*)cbContext;
    switch (eventId) {
    case sdrplay_api_GainChange:
        //printf("sdrplay_api_EventCb: %s, tuner=%s gRdB=%d lnaGRdB=%d systemGain=%.2f\n", "sdrplay_api_GainChange", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", params->gainParams.gRdB, params->gainParams.lnaGRdB, params->gainParams.currGain);
        break;

    case sdrplay_api_PowerOverloadChange:
        //printf("sdrplay_api_PowerOverloadChange: tuner=%s powerOverloadChangeType=%s\n", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", (params->powerOverloadParams.powerOverloadChangeType == sdrplay_api_Overload_Detected) ? "sdrplay_api_Overload_Detected" : "sdrplay_api_Overload_Corrected");
        // Send update message to acknowledge power overload message received
        sdrplay_api_Update(rspDevice->getChosenDev()->dev, tuner, sdrplay_api_Update_Ctrl_OverloadMsgAck, sdrplay_api_Update_Ext1_None);
        break;

    case sdrplay_api_DeviceRemoved:
        //printf("sdrplay_api_EventCb: %s\n", "sdrplay_api_DeviceRemoved");
        break;

    default:
        //printf("sdrplay_api_EventCb: %d, unknown event\n", eventId);
        break;
    }
}

void RSPDevice::sendFreqToDevice() {
    deviceParams->rxChannelA->tunerParams.rfFreq = sdrplay_api_RfFreqT { freq, 0 };
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_Frf, sdrplay_api_Update_Ext1_None);
    printf("Frequency set: %d\r\n", (int)freq);
    needToSendFreq = false;
}

void RSPDevice::sendGainToDevice() {
    deviceParams->rxChannelA->tunerParams.gain = sdrplay_api_GainT { gain, lnaState, 0 };
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_Gr, sdrplay_api_Update_Ext1_None);
    needToSendGain = false;
}

void RSPDevice::sendBasebandFilterToDevice() {
    deviceParams->rxChannelA->tunerParams.bwType = (sdrplay_api_Bw_MHzT)basebandFilter;
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_BwType, sdrplay_api_Update_Ext1_None);
    needToSendBasebandFilter = false;
}

void RSPDevice::setFreq(double freq) {
    this->freq = freq;
    needToSendFreq = true;
}

void RSPDevice::setGain(int gain, unsigned char lnaState) {
    this->gain = gain;
    this->lnaState = lnaState;
    needToSendGain = true;
}

void RSPDevice::setBasebandFilter(int filterWidth) {
    basebandFilter = filterWidth;
    needToSendBasebandFilter = true;
}

bool RSPDevice::isNeedToSendFreq() {
    return needToSendFreq;
}

bool RSPDevice::isNeedToSendGain() {
    return needToSendGain;
}

bool RSPDevice::isNeedToSendFilter() {
    return needToSendBasebandFilter;
}

float RSPDevice::prepareData(short val) {
    return (val / 32767.0f);
}
