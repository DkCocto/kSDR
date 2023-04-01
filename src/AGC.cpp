#include "AGC.h"

AGC::AGC(Config* config, SpectreHandler* specHandler) {
    this->specHandler = specHandler;
    this->config = config;
    amp = config->receiver.agc.lastAmp;
}

int count = 0;

float AGC::processNew(float signal) {
    if (signal * amp > config->receiver.agc.threshold) {
        float speed = (abs(amp - (config->receiver.agc.threshold / signal))) / ((config->receiver.agc.atackSpeedMs * config->outputSamplerate) / 1000.0f);
        atack(signal, speed);
        count = 0;
    } else {
        if (count > (config->receiver.agc.holdingTimeMs * config->outputSamplerate / 1000.0f)) {
            releaseAtack(signal, config->receiver.agc.releaseSpeed);
        }
    }
    count++;

    if (amp > 100) amp = 100;

    //Выводим коэффициант усиления в модель-контейнер отображения и хранения данных
    //Display::instance->viewModel->amp = amp;
    config->receiver.agc.lastAmp = amp;
    return signal * amp;
}

void AGC::atack(float signal, float speed) {
    amp -= speed;
}

void AGC::releaseAtack(float signal, float speed) {
    amp += speed;
}

float AGC::getAmp() {
    return amp;
}