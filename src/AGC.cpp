#include "AGC.h"


KalmanFilter kf(10, 0.000000001);

AGC::AGC(FFTSpectreHandler* fftSpectreHandler) {
    this->fftSpectreHandler = fftSpectreHandler;
    //memset(buf, 0, sizeof(double) * bufLen);
}

double AGC::process(double signal) {
  

    double signalAbs = abs(signal);

    if (signalAbs > 1 || isnan(signal)) return 0;


    /*delta = abs(signalAbs - savedValue);

    if (delta > 0.0045) {
        printf("KAKA\r\n");
        amp = threshold / kf.filter(100000 * signalAbs);
    }
    else {
        amp = threshold / kf.filter(signalAbs);
    }

    savedValue = signalAbs;*/
    amp = averageAmp->process(threshold / kf.filter(signalAbs));
    

    ////Выводим коэффициант усиления в модель-контейнер отображения и хранения данных
    Display::instance->viewModel->amp = amp;

    return signal * amp;


    //if (count == bufLen) {
    //    //averageAmp->process(max);
    //    amp = threshold / kf.filter(max);
    //    
    //    count = 0;
    //    max = 0;
    //}

    //if (signalAbs > max) {
    //    max = signalAbs;
    //}

    //double outputSignal = signal * amp;
    ////printf("%f %f\r\n", amp, averageAmp->getAvg() - (0.01 * (abs(outputSignal) - threshold)) * averageAmp->getAvg());
    //if (abs(outputSignal) > threshold) {
    //    //printf("%f %f\r\n", (abs(outputSignal) - threshold), averageAmp->getAvg());
    //    amp = threshold / averageAmp->process(abs(averageAmp->getAvg() - (0.1 * abs(outputSignal) - threshold) * averageAmp->getAvg()));
    //    outputSignal = signal * amp;
    //}

    //if (outputSignal > 2 * threshold) {
    //    amp -= 0.001;
    //    outputSignal = signal * amp;
    //}

    //count++;

    //buf.insert(buf.begin(), signal);

    //if (buf.size() > bufLen) {
    //    max = 0;

    //    for (int i = 0; i < buf.size(); i++) {
    //        if (abs(buf[i]) > max) max = abs(buf[i]);
    //    }

    //    max = averageAmp->process(max);
    //    amp = threshold / max;

    //    //Выводим коэффициант усиления в модель-контейнер отображения и хранения данных
    //    Display::instance->viewModel.amp = amp;

    //    double s = buf[buf.size() - 1];
    //    buf.pop_back();

    //    return s * amp;
    //}

    //double signalAbs = abs(signal);
    //delta = signalAbs - savedValue;

    //if (delta >= threshold) {
    //    //savedDelta = signalAbs - savedValue;
    //    amp = threshold / signalAbs;
    //    count = 0;
    //    max = 0;
    //} else {
    //    if (count == bufLen) {
    //        max = averageAmp->process(max);
    //        amp = threshold / max;

    //        count = 0;
    //        max = 0;
    //        savedDelta = 0;
    //    }

    //    if (signalAbs > max) {
    //        max = signalAbs;
    //    }
    //    count++;
    //}

    //savedValue = signalAbs;



}

//Расчет и возврат среднего значения амплитуды сигнала на области приёма. Базироваться в расчетах будем на спектре. 
double AGC::canculateAmplitudeAverage() {
    //Display* display = Display::instance;
    //if (display != NULL && display->receiver != NULL) {
    //    if (display->receiver->initialized) {
    //        ReceiverLogic::ReceiveBinArea r = display->receiver->getReceiveBinsArea(display->viewModel.filterWidth, display->viewModel.receiverMode);
    //        
    //        float* spectre = fftSpectreHandler->getOutput();
    //        //float sum = 0.0;
    //        int len = r.B - r.A;

    //        if (len == 0) return abs(spectre[0]);

    //        float max = abs(spectre[r.A]);

    //        for (int i = r.A; i < r.B; i++) {
    //            if (abs(spectre[i]) > max) max = abs(spectre[i]);
    //            //sum += abs(spectre[i]);
    //        }
    //        
    //        //return sum / (float)len;
    //        //printf("%f\r\n", max);
    //        return max;
    //        //printf("%i %i\r\n", r.A, r.B);
    //    }
    //}
    return 0.0;
}

double AGC::getAmp() {
    return amp;
}