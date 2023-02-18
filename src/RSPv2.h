#pragma once

#include "Env.h"
#include "sdrplay\sdrplay_api.h"
#include "Device.h"

class RSPv2: Device {

    sdrplay_api_DeviceT* chosenDevice = NULL;

    static void StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);
    static void StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);

    static void EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext);

public:

    void init();
    void closeApi();
};