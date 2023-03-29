#include "AGC.h"

AGC::AGC(Config* config, SpectreHandler* specHandler) {
    this->specHandler = specHandler;
    this->config = config;
    amp = config->receiver.agc.lastAmp;
}

int count = 0;

double AGC::processNew(double signal) {
    if (signal * amp > config->receiver.agc.threshold) {
        double speed = (abs(amp - (config->receiver.agc.threshold / signal))) / ((config->receiver.agc.atackSpeedMs * config->outputSamplerate) / 1000.0);
        atack(signal, speed);
        count = 0;
    } else {
        if (count > (config->receiver.agc.holdingTimeMs * config->outputSamplerate / 1000.0)) {
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

void AGC::atack(double signal, double speed) {
    amp -= speed;
}

void AGC::releaseAtack(double signal, double speed) {
    amp += speed;
}

double AGC::getAmp() {
    return amp;
}