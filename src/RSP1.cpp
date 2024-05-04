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
    Config* config = (Config*)cbContext;
    //RSP1* rsp1 = (RSP1*)config->device;

    RSP1* rsp1 = nullptr;
    if (rsp1 == nullptr) return;

    for (int i = 0; i < numSamples; i++) {
        if (i % config->inputSamplerateDivider == 0) {
            rsp1->cb->write(xi[i] / 32767.0);
            rsp1->cb->write(xq[i] / 32767.0);
        }
    }

    if (rsp1->isNeedToSetGain() || rsp1->isNeedToSetLna()) rsp1->setGain(rsp1->viewModel->rspModel.gain, rsp1->viewModel->rspModel.lna);

    if (rsp1->isNeedToSetFreq()) rsp1->setFreq(rsp1->viewModel->centerFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));

}

//void RSP1::gainCallback(unsigned int gRdB, unsigned int lnaGRdB, void* cbContext) {
    //printf("%d\r\n", gRdB); 
    //RSP1* rsp1 = (RSP1*)cbContext;
    //rsp1->viewModel->gainFromDevice = gRdB;
    //return;
//}

bool RSP1::init() {
    viewModel = Display::instance->getViewModel();

    mir_sdr_DeviceT devices[4];
    unsigned int numDevs;
    int devAvail = 0;
    int device = 0;
    float ppmOffset = 0.0;
    mir_sdr_RSPII_AntennaSelectT ant = mir_sdr_RSPII_ANTENNA_A;
    mir_sdr_SetGrModeT grMode = mir_sdr_USE_SET_GR_ALT_MODE;
    mir_sdr_ErrT r;
    int gainR = viewModel->rspModel.gain;
    //uint32_t samp_rate = config->currentWorkingInputSamplerate * config->inputSamplerateDivider;
    uint32_t frequency = config->startFrequency + ((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0);
    savedFreq = frequency;
    int bwkHz = mir_sdr_BW_0_600;
    int ifkHz = 0;
    int rspLNA = !viewModel->rspModel.lna;
    int gRdBsystem = gainR;
    long setPoint = viewModel->receiverMode;
    int refClk = 0;
    int notchEnable = 0;
    int biasT = 0;

    //mir_sdr_DebugEnable(1);
    mir_sdr_GetDevices(&devices[0], &numDevs, 4);

    for (int i = 0; i < numDevs; i++) {
        if (devices[i].devAvail == 1) {
            devAvail++;
        }
    }

    if (devAvail == 0) {
        status->err.append("ERROR: No RSP devices available.");
        status->isOK = false;
        status->isInitProcessOccured = true;
        return status->isOK;
    }

    if (devices[device].devAvail != 1) {
        status->err.append("ERROR: RSP selected (%d) is not available.");
        //fprintf(stderr, "ERROR: RSP selected (%d) is not available.\n", (device + 1));
        status->isOK = false;
        status->isInitProcessOccured = true;
        return status->isOK;
    }

    mir_sdr_SetDeviceIdx(device);

    devModel = devices[device].hwVer;

    mir_sdr_SetPpm(ppmOffset);

    grMode = mir_sdr_USE_RSP_SET_GR;
    if (devModel == 1) grMode = mir_sdr_USE_SET_GR_ALT_MODE;

    mir_sdr_DecimateControl(1, config->rsp.deviceDecimationFactor, 0);

    r = mir_sdr_StreamInit(&gainR, (config->rsp.deviceSamplingRate / 1e6), (frequency / 1e6),
        (mir_sdr_Bw_MHzT)bwkHz, (mir_sdr_If_kHzT)ifkHz, rspLNA, &gRdBsystem,
        grMode, &samplesPerPacket, RSP1::streamCallback, NULL, (void*)config);

    if (r != mir_sdr_Success) {
        status->err.append("Failed to start SDRplay RSP device.");
        //fprintf(stderr, "Failed to start SDRplay RSP device.\n");
        status->isOK = false;
        status->isInitProcessOccured = true;
        return status->isOK;
    }

    status->isOK = true;
    status->isInitProcessOccured = true;

    return status->isOK;
    //mir_sdr_AgcControl(mir_sdr_AGC_5HZ, setPoint, 0, 0, 0, 0, rspLNA);
}

void RSP1::setFreq(double freq) {
    mir_sdr_SetRf(freq, 1, 0);
    printf("Frequency set: %d\r\n", (int)freq);
    savedFreq = freq;
}

//-60 .. 0
void RSP1::setGain(int gain, bool lna) {
    //mir_sdr_AgcControl(mir_sdr_AGC_5HZ, gain, 0, 0, 0, 0, 0);
    //mir_sdr_SetGr(gain, 1, 0);

    mir_sdr_RSP_SetGr(gain, lna, 1, 0);
    //mir_sdr_AgcControl(mir_sdr_AGC_DISABLE, gain, 0, 0, 0, 0, 0);
    savedGain = gain;
    savedLna = lna;
}

void RSP1::disableGain() {
    mir_sdr_AgcControl(mir_sdr_AGC_DISABLE, viewModel->rspModel.gain, 0, 0, 0, 0, 0);
}

bool RSP1::isNeedToSetFreq() {
    return savedFreq != (viewModel->centerFrequency + (double)((config->receiver.enableFrequencyShift == true) ? config->receiver.frequencyShift : 0));
}

bool RSP1::isNeedToSetGain() {
    return savedGain != viewModel->rspModel.gain;
}

bool RSP1::isNeedToSetLna() {
    return savedLna != viewModel->rspModel.lna;
}
