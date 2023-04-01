#pragma once

#include "Env.h"
#include "sdrplay\sdrplay_api.h"
#include "Device.h"
#include "Config.h"
#include "CircleBuffer.h"
#include "Display.h"

class RSPv2: public Device {

    sdrplay_api_DeviceT devs[6];
    sdrplay_api_DeviceT* chosenDevice = NULL;
    sdrplay_api_DeviceParamsT* deviceParams = NULL;
    sdrplay_api_CallbackFnsT cbFns;

    static void StreamACallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);
    static void StreamBCallback(short* xi, short* xq, sdrplay_api_StreamCbParamsT* params, unsigned int numSamples, unsigned int reset, void* cbContext);

    static void EventCallback(sdrplay_api_EventT eventId, sdrplay_api_TunerSelectT tuner, sdrplay_api_EventParamsT* params, void* cbContext);

    Config* config;
    CircleBuffer* cb;

    ViewModel* viewModel;

    double savedFreq;
    int savedLna;
    int savedGain;
    int savedBasebandFilter;

public:

    RSPv2(Config* config, CircleBuffer* cb);
    ~RSPv2();
    bool init();
    void closeApi();
    void updateFreq();
    void updateGain();
    void updateBasebandFilter();
    bool isNeedToFreq();
    bool isNeedToGain();
    bool isNeedToLna();
    bool isNeedToBasebandFilter();

};