#pragma once

#include "CircleBuffer.h"
#include "Config.h"
#include "ViewModel.h"
#include "Device.h"

class RSP1: public Device {

    Config* config;

    double savedFreq;
    int savedGain;
    bool savedLna;

    bool isNeedToSetFreq();
    bool isNeedToSetGain();
    bool isNeedToSetLna();

    ViewModel* viewModel;

public: 
    CircleBuffer* cb;

    RSP1(Config* config, CircleBuffer* cb);

    ~RSP1();

    static void streamCallback(short* xi, short* xq, unsigned int firstSampleNum,
        int grChanged, int rfChanged, int fsChanged, unsigned int numSamples,
        unsigned int reset, unsigned int hwRemoved, void* cbContext);

    static void gainCallback(unsigned int gRdB, unsigned int lnaGRdB, void* cbContext);

    bool init();

    void setFreq(double freq);
    void setGain(int gain, bool lna);
    void disableGain();

};