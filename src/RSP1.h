#pragma once

#include "Env.h"
#include "../include/mirsdr/mir_sdr.h"
#include "CircleBuffer.h"
#include "Config.h"

class RSP1 {

    CircleBuffer* cb;

    Config* config;

public: 
    
    RSP1(Config* config, CircleBuffer* cb);

    ~RSP1();

    static void streamCallback(short* xi, short* xq, unsigned int firstSampleNum,
        int grChanged, int rfChanged, int fsChanged, unsigned int numSamples,
        unsigned int reset, unsigned int hwRemoved, void* cbContext);

    static void gainCallback(unsigned int gRdB, unsigned int lnaGRdB, void* cbContext);

    void init();

};