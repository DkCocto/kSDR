#include "AGC.h"
#include <ctime>
#include "Compressor/SimpleComp.h"

//KalmanFilter kf(1, 0.00000009);

AGC::AGC(Config* config, FFTSpectreHandler* fftSpectreHandler) {
    this->fftSpectreHandler = fftSpectreHandler;
    this->config = config;
    /*sc.setSampleRate(config->outputSamplerate);
    sc.setAttack(10);
    sc.setRelease(1000);
    sc.setThresh(1);
    sc.setRatio(10);
    sc.initRuntime();*/
    //memset(buf, 0, sizeof(double) * bufLen);
}

/*double savedAmp = 0;

int savedClock = 0;*/

/*double AGC::processNew(double signal) {
    double signalAbs = abs(signal);

    if (signalAbs > 1 || isnan(signal)) return 0;

    amp = avAmp->process(threshold / signalAbs);

    if (savedAmp < amp) {
        savedAmp = amp;
        savedClock = clock();
    } else {
        if (clock() - savedClock > 1000) {
            savedAmp -= 0.1;
        }
    }

    Display::instance->viewModel->amp = amp;

    return signal * savedAmp;
}*/

/*double AGC::process(double signal) {
    double signalAbs = abs(signal);

    if (signalAbs > 1 || isnan(signal)) return 0;

    amp = averageAmp->process(threshold / kf.filter(signalAbs));
    
    if (amp > 100) amp = 100;

    ////Выводим коэффициант усиления в модель-контейнер отображения и хранения данных
    Display::instance->viewModel->amp = amp;

    return signal * amp;
}*/

int count = 0;

double AGC::processNew(double signal) {
    if (signal * amp > config->receiver.agc.threshold) {
        double speed = (abs(amp - (config->receiver.agc.threshold / signal))) / ((config->receiver.agc.atackSpeedMs * config->outputSamplerate) / 1000);
        atack(signal, speed);
        count = 0;
    } else {
        if (count > (config->receiver.agc.holdingTimeMs * config->outputSamplerate / 1000)) {
            releaseAtack(signal, config->receiver.agc.releaseSpeed);
        }
    }

    count++;

    if (amp > 100) amp = 100;

    //Выводим коэффициант усиления в модель-контейнер отображения и хранения данных
    Display::instance->viewModel->amp = amp;

    return signal * amp;
}

void AGC::atack(double signal, double speed) {
    amp -= speed;
}

void AGC::releaseAtack(double signal, double speed) {
    amp += speed;
}


/*KalmanFilter maxdBKalman(10, 0.00000001);
//Расчет и возврат максимального значения амплитуды сигнала на области приёма. Базироваться в расчетах будем на спектре. 
double AGC::canculateAmplitudeAverage() {
    Display* display = Display::instance;
    if (display != NULL && display->receiver != NULL) {
        if (display->receiver->initialized) {
            ReceiverLogic::ReceiveBinArea r = display->receiver->getReceiveBinsArea(display->viewModel->filterWidth, display->viewModel->receiverMode);
            
            float* spectre = fftSpectreHandler->getOutput();
            float sum = 0.0;
            int len = r.B - r.A;

            if (len == 0) return spectre[0];

            float max = spectre[r.A];
            //Utils::printArray(spectre, 64);
            //printf("%i %i\r\n", r.A, r.B);

            display->viewModel->serviceField1 = r.A;
            display->viewModel->serviceField2 = r.B;

            for (int i = r.A; i < r.B; i++) {
                if (spectre[i] > max) max = spectre[i];
                //sum += spectre[i];
            }
            
            //return sum / (float)len;
            //printf("%f\r\n", max);
            //return maxdBKalman.filter(sum / (float)len);
            return maxdBKalman.filter(max);
        }
    }
    return 0.0;
}*/

double AGC::getAmp() {
    return amp;
}