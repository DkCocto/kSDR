#include "RSP1.h"

int devModel = 1;
int samplesPerPacket;

#define DEFAULT_SAMPLE_RATE		2000000

RSP1::RSP1(Config* config, CircleBuffer* cb) {
    this->config = config;
    this->cb = cb;
}

RSP1::~RSP1() {
    mir_sdr_StreamUninit();
    mir_sdr_ReleaseDeviceIdx();
}

void RSP1::streamCallback(short* xi, short* xq, unsigned int firstSampleNum, int grChanged, int rfChanged, int fsChanged, unsigned int numSamples, unsigned int reset, unsigned int hwRemoved, void* cbContext) {
    RSP1* rsp1 = (RSP1*)cbContext;

    int j = 0;
    int k = 0;
    for (int i = 0; i < numSamples * 2; i++) {
        if (i % 2 == 0) {
            rsp1->cb->write(xi[j] / 32768.0);
            j++;
        } else {
            rsp1->cb->write(xq[k] / 32768.0);
            k++;
        }

    }


    if (rsp1->isNeedToSetFreq()) rsp1->setFreq(rsp1->viewModel->frequency);

    if (rsp1->viewModel->gainControl) {
        if (rsp1->isNeedToSetGain()) rsp1->setGain(rsp1->viewModel->gain);
    } else {
        rsp1->disableGain();
    }

}

void RSP1::gainCallback(unsigned int gRdB, unsigned int lnaGRdB, void* cbContext) {
    return;
}

void RSP1::init() {
    viewModel = Display::instance->viewModel;
    mir_sdr_DeviceT devices[4];
    unsigned int numDevs;
    int devAvail = 0;
    int device = 0;
    float ppmOffset = 0.0;
    mir_sdr_RSPII_AntennaSelectT ant = mir_sdr_RSPII_ANTENNA_A;
    mir_sdr_SetGrModeT grMode = mir_sdr_USE_SET_GR_ALT_MODE;
    mir_sdr_ErrT r;
    int gainR = 50;
    uint32_t samp_rate = config->inputSamplerate * config->inputSamplerateDivider;
    uint32_t frequency = config->startFrequency;
    savedFreq = frequency;
    int bwkHz = 1536;
    int ifkHz = 0;
    int rspLNA = 0;
    int gRdBsystem;
    long setPoint = viewModel->receiverMode;
    int refClk = 0;
    int notchEnable = 0;
    int biasT = 0;

    mir_sdr_DebugEnable(1);
    mir_sdr_GetDevices(&devices[0], &numDevs, 4);

    for (int i = 0; i < numDevs; i++) {
        if (devices[i].devAvail == 1) {
            devAvail++;
        }
    }

    if (devAvail == 0) {
        fprintf(stderr, "ERROR: No RSP devices available.\n");
        exit(1);
    }

    if (devices[device].devAvail != 1) {
        fprintf(stderr, "ERROR: RSP selected (%d) is not available.\n", (device + 1));
        exit(1);
    }

    mir_sdr_SetDeviceIdx(device);

    devModel = devices[device].hwVer;

    mir_sdr_SetPpm(ppmOffset);

    grMode = mir_sdr_USE_RSP_SET_GR;
    if (devModel == 1) grMode = mir_sdr_USE_SET_GR_ALT_MODE;

    mir_sdr_DecimateControl(1, config->inputSamplerateDivider, 1);

    r = mir_sdr_StreamInit(&gainR, ((config->inputSamplerate * config->inputSamplerateDivider) / 1e6), (frequency / 1e6),
        (mir_sdr_Bw_MHzT)bwkHz, (mir_sdr_If_kHzT)ifkHz, rspLNA, &gRdBsystem,
        grMode, &samplesPerPacket, RSP1::streamCallback, RSP1::gainCallback, (void*)this);

    if (r != mir_sdr_Success) {
        fprintf(stderr, "Failed to start SDRplay RSP device.\n");
        exit(1);
    }

    mir_sdr_AgcControl(mir_sdr_AGC_DISABLE, setPoint, 0, 0, 0, 0, rspLNA);

}

void RSP1::setFreq(double freq) {
    mir_sdr_SetRf(freq, 1, 0);
    savedFreq = freq;
}

//-60 .. 0
void RSP1::setGain(int gain) {
    mir_sdr_AgcControl(mir_sdr_AGC_5HZ, gain, 0, 0, 0, 0, 0);
    //mir_sdr_AgcControl(mir_sdr_AGC_DISABLE, gain, 0, 0, 0, 0, 0);
    savedGain = gain;
}

void RSP1::disableGain() {
    mir_sdr_AgcControl(mir_sdr_AGC_DISABLE, -30, 0, 0, 0, 0, 0);
}

bool RSP1::isNeedToSetFreq() {
    return savedFreq != viewModel->frequency;
}

bool RSP1::isNeedToSetGain() {
    return savedGain != viewModel->gain;
}
