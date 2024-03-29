#include "RSPv2.h"

void RSPv2::StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    // Process stream callback data here
    Config* config = (Config*)cbContext;
    //RSPv2* rspv2 = (RSPv2*)config->device;
    RSPv2* rspv2 = nullptr;
    if (rspv2 == nullptr) return;
    if (reset) {
        printf("sdrplay_api_StreamACallback: numSamples=%d\n", numSamples);
    }

    for (int i = 0; i < numSamples; i++) {
        if (i % config->inputSamplerateDivider == 0) {
            rspv2->cb->write(xi[i] / 32767.0);
            rspv2->cb->write(xq[i] / 32767.0);
        }
    }

    if (rspv2->isNeedToFreq()) rspv2->updateFreq();

    if (rspv2->isNeedToGain() || rspv2->isNeedToLna()) rspv2->updateGain();
    if (rspv2->isNeedToBasebandFilter()) rspv2->updateBasebandFilter();
}

void RSPv2::StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    if (reset) printf("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);

    // Process stream callback data here - this callback will only be used in dual tuner mode
}

void RSPv2::EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext) {
    Config* config = (Config*)cbContext;
    //RSPv2* rspv2 = (RSPv2*)config->device;
    RSPv2* rspv2 = nullptr;
    if (rspv2 == nullptr) return;
    switch (eventId) {
        case sdrplay_api_GainChange:
            //printf("sdrplay_api_EventCb: %s, tuner=%s gRdB=%d lnaGRdB=%d systemGain=%.2f\n", "sdrplay_api_GainChange", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", params->gainParams.gRdB, params->gainParams.lnaGRdB, params->gainParams.currGain);
            break;

        case sdrplay_api_PowerOverloadChange:
            //printf("sdrplay_api_PowerOverloadChange: tuner=%s powerOverloadChangeType=%s\n", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", (params->powerOverloadParams.powerOverloadChangeType == sdrplay_api_Overload_Detected) ? "sdrplay_api_Overload_Detected" : "sdrplay_api_Overload_Corrected");
            // Send update message to acknowledge power overload message received
            sdrplay_api_Update(rspv2->chosenDevice->dev, tuner, sdrplay_api_Update_Ctrl_OverloadMsgAck, sdrplay_api_Update_Ext1_None);
            break;

        case sdrplay_api_DeviceRemoved:
            //printf("sdrplay_api_EventCb: %s\n", "sdrplay_api_DeviceRemoved");
            break;

        default:
            //printf("sdrplay_api_EventCb: %d, unknown event\n", eventId);
            break;
    }
}

RSPv2::RSPv2(Config* config, CircleBuffer* cb) {
    this->config = config;
    this->cb = cb;
}

RSPv2::~RSPv2() {
    if (status->isOK) {
        sdrplay_api_Uninit(chosenDevice->dev);
        sdrplay_api_ReleaseDevice(chosenDevice);
    }
    sdrplay_api_UnlockDeviceApi();
    closeApi();
}

bool RSPv2::init() {
    viewModel = Display::instance->getViewModel();

    unsigned int ndev;
    int i;
    float ver = 0.0;
    sdrplay_api_ErrT err;
    sdrplay_api_RxChannelParamsT* chParams;

    unsigned int chosenIdx = 0;

    if ((err = sdrplay_api_Open()) != sdrplay_api_Success) {
        status->err.append("sdrplay_api_Open failed: ");
        status->err.append(sdrplay_api_GetErrorString(err));
        status->isOK = false;
        status->isInitProcessOccured = true;
        return status->isOK;
    } else {

        if ((err = sdrplay_api_DebugEnable(NULL, (sdrplay_api_DbgLvl_t)1)) != sdrplay_api_Success) {
            status->err.append("sdrplay_api_DebugEnable failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            return status->isOK;
        }

        if ((err = sdrplay_api_ApiVersion(&ver)) != sdrplay_api_Success) {
            status->err.append("sdrplay_api_ApiVersion failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            return status->isOK;
        }

        if (ver != SDRPLAY_API_VERSION) {
            status->err.append("API version don't match");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            closeApi();
            return status->isOK;
            //printf("API version don't match (local=%.2f dll=%.2f)\n", SDRPLAY_API_VERSION, ver);
        }

        sdrplay_api_LockDeviceApi();

        // Fetch list of available devices
        if ((err = sdrplay_api_GetDevices(devs, &ndev, sizeof(devs) / sizeof(sdrplay_api_DeviceT))) != sdrplay_api_Success) {
            status->err.append("sdrplay_api_GetDevices failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            sdrplay_api_UnlockDeviceApi();
            closeApi();
            return status->isOK;
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
            status->err.append("sdrplay_api_SelectDevice failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;

            //printf("sdrplay_api_SelectDevice failed %s\n", sdrplay_api_GetErrorString(err));
            sdrplay_api_UnlockDeviceApi();
            sdrplay_api_Close();
            return status->isOK;
        }

        sdrplay_api_UnlockDeviceApi();

        // Retrieve device parameters so they can be changed if wanted
        if ((err = sdrplay_api_GetDeviceParams(chosenDevice->dev, &deviceParams)) != sdrplay_api_Success) {
            status->err.append("sdrplay_api_GetDeviceParams failed failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            sdrplay_api_Close();
            return status->isOK;
        }

        // Check for NULL pointers before changing settings
        if (deviceParams == NULL) {
            status->err.append("sdrplay_api_GetDeviceParams returned NULL deviceParams pointer");
            status->isOK = false;
            status->isInitProcessOccured = true;
            sdrplay_api_Close();
            return status->isOK;
        }

        deviceParams->devParams->fsFreq.fsHz = config->rsp.deviceSamplingRate;
        deviceParams->rxChannelA->ctrlParams.decimation = sdrplay_api_DecimationT {1, (unsigned char)config->rsp.deviceDecimationFactor, 0};

        // Configure tuner parameters (depends on selected Tuner which set of parameters to use)
        chParams = (chosenDevice->tuner == sdrplay_api_Tuner_B) ? deviceParams->rxChannelB : deviceParams->rxChannelA;
        if (chParams != NULL) {
            chParams->tunerParams.rfFreq.rfHz = config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);
            chParams->tunerParams.bwType = sdrplay_api_BW_5_000;

            chParams->tunerParams.ifType = sdrplay_api_IF_Zero;

            chParams->tunerParams.gain.gRdB = config->rsp.gain;
            chParams->tunerParams.gain.LNAstate = config->rsp.lna;

            // Disable AGC
            chParams->ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE; 
        } else {
            status->err.append("sdrplay_api_GetDeviceParams returned NULL chParams pointer");
            status->isOK = false;
            status->isInitProcessOccured = true;
            sdrplay_api_Close();
            return status->isOK;
        }

        cbFns.StreamACbFn = RSPv2::StreamACallback;
        cbFns.StreamBCbFn = RSPv2::StreamBCallback;
        cbFns.EventCbFn = RSPv2::EventCallback;

        if ((err = sdrplay_api_Init(chosenDevice->dev, &cbFns, (void*)config)) != sdrplay_api_Success) {
            status->err.append("sdrplay_api_Init failed: ");
            status->err.append(sdrplay_api_GetErrorString(err));
            status->isOK = false;
            status->isInitProcessOccured = true;
            sdrplay_api_Close();
            return status->isOK;
        }
    }
    status->isOK = true;
    status->isInitProcessOccured = true;
    return true;
}

void RSPv2::closeApi() {
    sdrplay_api_Close();
}

void RSPv2::updateFreq() {
    deviceParams->rxChannelA->tunerParams.rfFreq = sdrplay_api_RfFreqT{ (double)(viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0)), 0 };
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_Frf, sdrplay_api_Update_Ext1_None);
    printf("Frequency set: %d\r\n", viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
    savedFreq = viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);
}

void RSPv2::updateGain() {
    deviceParams->rxChannelA->tunerParams.gain = sdrplay_api_GainT{ viewModel->rspModel.gain, (unsigned char)viewModel->rspModel.lna, 0 };
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_Gr, sdrplay_api_Update_Ext1_None);
    savedGain = viewModel->rspModel.gain;
    savedLna = viewModel->rspModel.lna;
}

void RSPv2::updateBasebandFilter() {
    /*sdrplay_api_BW_Undefined = 0,
        sdrplay_api_BW_0_200 = 200,
        sdrplay_api_BW_0_300 = 300,
        sdrplay_api_BW_0_600 = 600,
        sdrplay_api_BW_1_536 = 1536,
        sdrplay_api_BW_5_000 = 5000,
        sdrplay_api_BW_6_000 = 6000,
        sdrplay_api_BW_7_000 = 7000,
        sdrplay_api_BW_8_000 = 8000*/
    /*sdrplay_api_Bw_MHzT bwFreq = sdrplay_api_BW_Undefined;
    switch (config->rsp.basebandFilter) {
        case 0:
            bwFreq = sdrplay_api_BW_Undefined;
    }*/
    deviceParams->rxChannelA->tunerParams.bwType = (sdrplay_api_Bw_MHzT)config->rsp.basebandFilter;
    sdrplay_api_Update(chosenDevice->dev, chosenDevice->tuner, sdrplay_api_Update_Tuner_BwType, sdrplay_api_Update_Ext1_None);
    savedBasebandFilter = config->rsp.basebandFilter;
}

bool RSPv2::isNeedToFreq() {
    return savedFreq != (viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
}

bool RSPv2::isNeedToGain() {
    return savedGain != (viewModel->rspModel.gain);
}

bool RSPv2::isNeedToLna() {
    return savedLna != viewModel->rspModel.lna;
}

bool RSPv2::isNeedToBasebandFilter() {
    return savedBasebandFilter != config->rsp.basebandFilter;
}