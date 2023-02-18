#include "RSPv2.h"

void RSPv2::StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    if (reset)
        printf("sdrplay_api_StreamACallback: numSamples=%d\n", numSamples);

    // Process stream callback data here

    return;
}

void RSPv2::StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext) {
    if (reset)
        printf("sdrplay_api_StreamBCallback: numSamples=%d\n", numSamples);

    // Process stream callback data here - this callback will only be used in dual tuner mode

    return;
}

void RSPv2::EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext) {
    switch (eventId)
    {
        case sdrplay_api_GainChange:
            printf("sdrplay_api_EventCb: %s, tuner=%s gRdB=%d lnaGRdB=%d systemGain=%.2f\n", "sdrplay_api_GainChange", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", params->gainParams.gRdB, params->gainParams.lnaGRdB, params->gainParams.currGain);
            break;

        /*case sdrplay_api_PowerOverloadChange:
            printf("sdrplay_api_PowerOverloadChange: tuner=%s powerOverloadChangeType=%s\n", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", (params->powerOverloadParams.powerOverloadChangeType == sdrplay_api_Overload_Detected) ? "sdrplay_api_Overload_Detected" : "sdrplay_api_Overload_Corrected");
            // Send update message to acknowledge power overload message received
            sdrplay_api_Update(chosenDevice->dev, tuner, sdrplay_api_Update_Ctrl_OverloadMsgAck, sdrplay_api_Update_Ext1_None);
            break;

        case sdrplay_api_RspDuoModeChange:
            printf("sdrplay_api_EventCb: %s, tuner=%s modeChangeType=%s\n", "sdrplay_api_RspDuoModeChange", (tuner == sdrplay_api_Tuner_A) ? "sdrplay_api_Tuner_A" : "sdrplay_api_Tuner_B", (params->rspDuoModeParams.modeChangeType == sdrplay_api_MasterInitialised) ? "sdrplay_api_MasterInitialised" :
                (params->rspDuoModeParams.modeChangeType == sdrplay_api_SlaveAttached) ? "sdrplay_api_SlaveAttached" :
                (params->rspDuoModeParams.modeChangeType == sdrplay_api_SlaveDetached) ? "sdrplay_api_SlaveDetached" :
                (params->rspDuoModeParams.modeChangeType == sdrplay_api_SlaveInitialised) ? "sdrplay_api_SlaveInitialised" :
                (params->rspDuoModeParams.modeChangeType == sdrplay_api_SlaveUninitialised) ? "sdrplay_api_SlaveUninitialised" :
                "unknown type");
            if (params->rspDuoModeParams.modeChangeType == sdrplay_api_MasterInitialised)
            {
                masterInitialised = 1;
            }
            if (params->rspDuoModeParams.modeChangeType == sdrplay_api_SlaveUninitialised)
            {
                slaveUninitialised = 1;
            }
            break;*/

        case sdrplay_api_DeviceRemoved:
            printf("sdrplay_api_EventCb: %s\n", "sdrplay_api_DeviceRemoved");
            break;

        default:
            printf("sdrplay_api_EventCb: %d, unknown event\n", eventId);
            break;
    }
}

void RSPv2::init() {

    sdrplay_api_DeviceT devs[6];
    unsigned int ndev;
    int i;
    float ver = 0.0;
    sdrplay_api_ErrT err;
    sdrplay_api_DeviceParamsT* deviceParams = NULL;
    sdrplay_api_CallbackFnsT cbFns;
    sdrplay_api_RxChannelParamsT* chParams;

    unsigned int chosenIdx = 0;

    if ((err = sdrplay_api_Open()) != sdrplay_api_Success) {
        printf("sdrplay_api_Open failed %s\n", sdrplay_api_GetErrorString(err));
        exit(-1);
    } else {

        if ((err = sdrplay_api_DebugEnable(NULL, (sdrplay_api_DbgLvl_t)1)) != sdrplay_api_Success) {
            printf("sdrplay_api_DebugEnable failed %s\n", sdrplay_api_GetErrorString(err));
            exit(-1);
        }

        if ((err = sdrplay_api_ApiVersion(&ver)) != sdrplay_api_Success) {
            printf("sdrplay_api_ApiVersion failed %s\n", sdrplay_api_GetErrorString(err));
            exit(-1);
        }

        if (ver != SDRPLAY_API_VERSION)
        {
            printf("API version don't match (local=%.2f dll=%.2f)\n", SDRPLAY_API_VERSION, ver);
            closeApi();
            exit(-1);
        }

        sdrplay_api_LockDeviceApi();

        // Fetch list of available devices
        if ((err = sdrplay_api_GetDevices(devs, &ndev, sizeof(devs) / sizeof(sdrplay_api_DeviceT))) != sdrplay_api_Success) {
            printf("sdrplay_api_GetDevices failed %s\n", sdrplay_api_GetErrorString(err));
            sdrplay_api_UnlockDeviceApi();
            closeApi();
            exit(-1);
        }
        printf("MaxDevs=%d NumDevs=%d\n", sizeof(devs) / sizeof(sdrplay_api_DeviceT), ndev);

        if (ndev > 0) {
            for (i = 0; i < (int)ndev; i++)
            {
                if (devs[i].hwVer == SDRPLAY_RSPduo_ID)
                    printf("Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x rspDuoMode=0x%.2x\n", i, devs[i].SerNo, devs[i].hwVer, devs[i].tuner, devs[i].rspDuoMode);
                else
                    printf("Dev%d: SerNo=%s hwVer=%d tuner=0x%.2x\n", i, devs[i].SerNo, devs[i].hwVer, devs[i].tuner);
            }
        }

        // Pick first device of any type
        for (i = 0; i < (int)ndev; i++)
        {
            chosenIdx = i;
            break;
        }

        printf("chosenDevice = %d\n", chosenIdx);
        chosenDevice = &devs[chosenIdx];

        // Select chosen device
        if ((err = sdrplay_api_SelectDevice(chosenDevice)) != sdrplay_api_Success)
        {
            printf("sdrplay_api_SelectDevice failed %s\n", sdrplay_api_GetErrorString(err));
            sdrplay_api_UnlockDeviceApi();
            sdrplay_api_Close();
            exit(-1);
        }

        sdrplay_api_UnlockDeviceApi();


        // Retrieve device parameters so they can be changed if wanted
        if ((err = sdrplay_api_GetDeviceParams(chosenDevice->dev, &deviceParams)) != sdrplay_api_Success) {
            printf("sdrplay_api_GetDeviceParams failed %s\n", sdrplay_api_GetErrorString(err));
            sdrplay_api_Close();
            exit(-1);
        }

        // Check for NULL pointers before changing settings
        if (deviceParams == NULL) {
            printf("sdrplay_api_GetDeviceParams returned NULL deviceParams pointer\n");
            sdrplay_api_Close();
            exit(-1);
        }

        // Configure tuner parameters (depends on selected Tuner which set of parameters to use)
        chParams = (chosenDevice->tuner == sdrplay_api_Tuner_B) ? deviceParams->rxChannelB : deviceParams->rxChannelA;
        if (chParams != NULL)
        {
            chParams->tunerParams.rfFreq.rfHz = 220000000.0;
            chParams->tunerParams.bwType = sdrplay_api_BW_1_536;

            chParams->tunerParams.ifType = sdrplay_api_IF_Zero;

            chParams->tunerParams.gain.gRdB = 40;
            chParams->tunerParams.gain.LNAstate = 5;

            // Disable AGC
            chParams->ctrlParams.agc.enable = sdrplay_api_AGC_DISABLE;
        } else {
            printf("sdrplay_api_GetDeviceParams returned NULL chParams pointer\n");
            sdrplay_api_Close();
            exit(-1);
        }

        cbFns.StreamACbFn = RSPv2::StreamACallback;
        cbFns.StreamBCbFn = RSPv2::StreamBCallback;
        cbFns.EventCbFn = RSPv2::EventCallback;

        if ((err = sdrplay_api_Init(chosenDevice->dev, &cbFns, NULL)) != sdrplay_api_Success) {
            printf("sdrplay_api_Init failed %s\n", sdrplay_api_GetErrorString(err));
            sdrplay_api_Close();
        }

        printf("chucha\r\n");
    }

}

void RSPv2::closeApi() {
    sdrplay_api_Close();
}
