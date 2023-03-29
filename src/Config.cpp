#include "Config.h"

Config::Config() {
    prepareConfiguration();
}

void Config::prepareConfiguration() {
    loadXml();
    loadMemory();
    initSettings();
}

void Config::initSettings() {
    inputChannelNumber = 1;
    outputChannelNumber = 1;

    switch (deviceType) {
    case DeviceType::RSP:
        inputSamplerate = (rsp.deviceSamplingRate / rsp.deviceDecimationFactor) / inputSamplerateDivider;
        break;
    case DeviceType::HACKRF:
        inputSamplerate = hackrf.deviceSamplingRate / inputSamplerateDivider;
        break;
    case DeviceType::RTL:
        inputSamplerate = rtl.deviceSamplingRate / inputSamplerateDivider;
        break;
    default:
        inputSamplerate = 4000000;
    }

    calcOutputSamplerate();

    bufferWriteAudioLen = (outputSamplerateDivider) / 4;

    readSoundProcessorBufferLen = fftLen; //fftLen 

    audioWriteFrameLen = (outputSamplerateDivider) / 4;

    circleBufferLen = inputSamplerate / 4;

    hilbertTransformLen = 255;
    polyphaseFilterLen = 2 * outputSamplerateDivider;

    fftBandwidth = (float)inputSamplerate / (float)fftLen;
}

Config::~Config() {
    save();
}

//Loading config from config file
void Config::loadXml() {

    tinyxml2::XMLDocument doc;

    doc.LoadFile(CONFIG_FILENAME);

    tinyxml2::XMLElement* pRootElement = doc.RootElement();

    if (NULL != pRootElement) {

        tinyxml2::XMLElement* pApp = pRootElement->FirstChildElement("App");
        if (NULL != pApp) {
            tinyxml2::XMLElement* pwindowWidth = pApp->FirstChildElement("windowWidth");
            app.winWidth = std::stoi(std::string(pwindowWidth->GetText()));

            tinyxml2::XMLElement* pwindowHeight = pApp->FirstChildElement("windowHeight");
            app.winHeight = std::stoi(std::string(pwindowHeight->GetText()));
        }

        tinyxml2::XMLElement* pDevice = pRootElement->FirstChildElement("Device");
        if (NULL != pDevice) {
            tinyxml2::XMLElement* ptype = pDevice->FirstChildElement("type");

            switch (std::stoi(std::string(ptype->GetText()))) {
                case 0:
                    deviceType = DeviceType::RSP;
                    break;
                case 1:
                    deviceType = DeviceType::HACKRF;
                    break;
                case 2:
                    deviceType = DeviceType::RTL;
                    break;
                default:
                    deviceType = DeviceType::HACKRF;
            }
        
            delayedDeviceType = deviceType;

            tinyxml2::XMLElement* psamplingRateDiv = pDevice->FirstChildElement("decimation");
            inputSamplerateDivider = std::stoi(std::string(psamplingRateDiv->GetText()));
            delayedInputSamplerateDivider = inputSamplerateDivider;

            tinyxml2::XMLElement* pHackRF = pDevice->FirstChildElement("HackRF");
            if (NULL != pHackRF) {
                tinyxml2::XMLElement* pdeviceSamplingRate = pHackRF->FirstChildElement("deviceSamplingRate");
                hackrf.deviceSamplingRate = std::stoi(std::string(pdeviceSamplingRate->GetText()));

                tinyxml2::XMLElement* pbasebandFilter = pHackRF->FirstChildElement("basebandFilter");
                hackrf.basebandFilter = std::stoi(std::string(pbasebandFilter->GetText()));

                tinyxml2::XMLElement* prxAmp = pHackRF->FirstChildElement("rxAmp");
                hackrf.rxAmp = std::stoi(std::string(prxAmp->GetText()));

                tinyxml2::XMLElement* plnaGain = pHackRF->FirstChildElement("lnaGain");
                hackrf.lnaGain = std::stoi(std::string(plnaGain->GetText()));

                tinyxml2::XMLElement* pvgaGain = pHackRF->FirstChildElement("vgaGain");
                hackrf.vgaGain = std::stoi(std::string(pvgaGain->GetText()));

                tinyxml2::XMLElement* ptxAmp = pHackRF->FirstChildElement("txAmp");
                hackrf.txAmp = std::stoi(std::string(ptxAmp->GetText()));
            }

            tinyxml2::XMLElement* pRSP = pDevice->FirstChildElement("RSP");
            if (NULL != pRSP) {
                tinyxml2::XMLElement* pdeviceSamplingRate = pRSP->FirstChildElement("deviceSamplingRate");
                rsp.deviceSamplingRate = std::stoi(std::string(pdeviceSamplingRate->GetText()));

                tinyxml2::XMLElement* pgain = pRSP->FirstChildElement("gain");
                rsp.gain = std::stoi(std::string(pgain->GetText()));

                tinyxml2::XMLElement* papi = pRSP->FirstChildElement("api");
                rsp.api = std::stoi(std::string(papi->GetText()));

                tinyxml2::XMLElement* pdeviceDecimationFactor = pRSP->FirstChildElement("deviceDecimationFactor");
                rsp.deviceDecimationFactor = std::stoi(std::string(pdeviceDecimationFactor->GetText()));

                tinyxml2::XMLElement* plna = pRSP->FirstChildElement("disableLna");
                rsp.lna = std::stoi(std::string(plna->GetText()));

                tinyxml2::XMLElement* pbasebandFilter = pRSP->FirstChildElement("basebandFilter");
                rsp.basebandFilter = std::stoi(std::string(pbasebandFilter->GetText()));
            }

            tinyxml2::XMLElement* pRTL = pDevice->FirstChildElement("RTL");
            if (NULL != pRTL) {
                tinyxml2::XMLElement* pdeviceSamplingRate = pRTL->FirstChildElement("deviceSamplingRate");
                rtl.deviceSamplingRate = std::stoi(std::string(pdeviceSamplingRate->GetText()));

                tinyxml2::XMLElement* pgain = pRTL->FirstChildElement("gain");
                rtl.gain = std::stoi(std::string(pgain->GetText()));
            }
        }

        tinyxml2::XMLElement* pReceiver = pRootElement->FirstChildElement("Receiver");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pstartFreq = pReceiver->FirstChildElement("startFreq");
            startFrequency = std::stof(std::string(pstartFreq->GetText()));

            tinyxml2::XMLElement* plastSelectedFreq = pReceiver->FirstChildElement("lastSelectedFreq");
            lastSelectedFreq = std::stof(std::string(plastSelectedFreq->GetText()));

            tinyxml2::XMLElement* pvolume = pReceiver->FirstChildElement("volume");
            volume = std::stof(std::string(pvolume->GetText()));

            tinyxml2::XMLElement* pfilterWidth = pReceiver->FirstChildElement("filterWidth");
            filterWidth = std::stoi(std::string(pfilterWidth->GetText()));

            tinyxml2::XMLElement* pmodulation = pReceiver->FirstChildElement("modulation");
            receiver.modulation = std::stoi(std::string(pmodulation->GetText()));

            tinyxml2::XMLElement* penableFrequencyShift = pReceiver->FirstChildElement("enableFrequencyShift");
            receiver.enableFrequencyShift = (std::stoi(std::string(penableFrequencyShift->GetText())) == 1) ? true : false;

            tinyxml2::XMLElement* pfrequencyShift = pReceiver->FirstChildElement("frequencyShift");
            receiver.frequencyShift = std::stoi(std::string(pfrequencyShift->GetText()));

            tinyxml2::XMLElement* pAgc = pReceiver->FirstChildElement("Agc");
            if (NULL != pAgc) {
                tinyxml2::XMLElement* pthreshold = pAgc->FirstChildElement("threshold");
                receiver.agc.threshold = std::stod(std::string(pthreshold->GetText()));
            
                tinyxml2::XMLElement* patackSpeedMs = pAgc->FirstChildElement("atackSpeedMs");
                receiver.agc.atackSpeedMs = std::stod(std::string(patackSpeedMs->GetText()));

                tinyxml2::XMLElement* pholdingTimeMs = pAgc->FirstChildElement("holdingTimeMs");
                receiver.agc.holdingTimeMs = std::stod(std::string(pholdingTimeMs->GetText()));

                tinyxml2::XMLElement* preleaseSpeed = pAgc->FirstChildElement("releaseSpeed");
                receiver.agc.releaseSpeed = std::stod(std::string(preleaseSpeed->GetText()));

                tinyxml2::XMLElement* plastAmp = pAgc->FirstChildElement("lastAmp");
                receiver.agc.lastAmp = std::stod(std::string(plastAmp->GetText()));
            }

        }

        tinyxml2::XMLElement* pWaterfall = pRootElement->FirstChildElement("Waterfall");
        if (NULL != pWaterfall) {
            tinyxml2::XMLElement* pmin = pWaterfall->FirstChildElement("min");
            waterfallMin = std::stof(std::string(pmin->GetText()));

            tinyxml2::XMLElement* pmax = pWaterfall->FirstChildElement("max");
            waterfallMax = std::stof(std::string(pmax->GetText()));

            tinyxml2::XMLElement* pspeed = pWaterfall->FirstChildElement("speed");
            waterfall.speed = std::stoi(std::string(pspeed->GetText()));
        }

        tinyxml2::XMLElement* pSpectre = pRootElement->FirstChildElement("Spectre");
        if (NULL != pSpectre) {
            tinyxml2::XMLElement* pratio = pSpectre->FirstChildElement("ratio");
            spectreRatio = std::stof(std::string(pratio->GetText()));
                
            tinyxml2::XMLElement* pspectreMin = pSpectre->FirstChildElement("min");
            spectreMin = std::stof(std::string(pspectreMin->GetText()));

            tinyxml2::XMLElement* pspeed = pSpectre->FirstChildElement("speed");
            spectre.spectreSpeed = std::stoi(std::string(pspeed->GetText()));

            tinyxml2::XMLElement* pspeed2 = pSpectre->FirstChildElement("speed2");
            spectre.spectreSpeed2 = std::stoi(std::string(pspeed2->GetText()));

            tinyxml2::XMLElement* pfftLen = pSpectre->FirstChildElement("fftLen");
            fftLen = std::stoi(std::string(pfftLen->GetText()));
            delayedFFTLen = fftLen;

            tinyxml2::XMLElement* pstartBin = pSpectre->FirstChildElement("startBin");
            startBin = std::stoi(std::string(pstartBin->GetText()));

            tinyxml2::XMLElement* pstopBin = pSpectre->FirstChildElement("stopBin");
            stopBin = std::stoi(std::string(pstopBin->GetText()));

            tinyxml2::XMLElement* pvisibleStartFreq = pSpectre->FirstChildElement("visibleStartFreq");
            spectre.visibleStartFreq = std::stoi(std::string(pvisibleStartFreq->GetText()));

            tinyxml2::XMLElement* pvisibleStopFreq = pSpectre->FirstChildElement("visibleStopFreq");
            spectre.visibleStopFreq = std::stoi(std::string(pvisibleStopFreq->GetText()));

            //Проверка на случай если fftLen изменилось, а границы спектра выходят за общую границу FFT
            if (startBin > (fftLen / 2) || stopBin > (fftLen / 2)) {
                startBin = 0;
                stopBin = (fftLen / 2) - 1;
            }

            tinyxml2::XMLElement* premoveDCBias = pSpectre->FirstChildElement("removeDCBias");
            removeDCBias = std::stoi(std::string(premoveDCBias->GetText()));

            tinyxml2::XMLElement* pstyle = pSpectre->FirstChildElement("style");
            spectre.style = std::stoi(std::string(pstyle->GetText()));

            tinyxml2::XMLElement* pcontourShowsPower = pSpectre->FirstChildElement("contourShowsPower");
            spectre.contourShowsPower = (std::stoi(std::string(pcontourShowsPower->GetText())) == 1) ? true : false;

            tinyxml2::XMLElement* pbottomCoeff = pSpectre->FirstChildElement("bottomCoeff");
            spectre.bottomCoeff = std::stof(std::string(pbottomCoeff->GetText()));

            tinyxml2::XMLElement* ptopCoeff = pSpectre->FirstChildElement("topCoeff");
            spectre.topCoeff = std::stof(std::string(ptopCoeff->GetText()));

            tinyxml2::XMLElement* psmoothing = pSpectre->FirstChildElement("hangAndDecay");
            spectre.hangAndDecay = (std::stoi(std::string(psmoothing->GetText())) == 1) ? true : false;

            tinyxml2::XMLElement* pdecaySpeedDelta = pSpectre->FirstChildElement("decaySpeedDelta");
            spectre.decaySpeedDelta = std::stof(std::string(pdecaySpeedDelta->GetText()));

            tinyxml2::XMLElement* pdecaySpeed = pSpectre->FirstChildElement("decaySpeed");
            spectre.decaySpeed = std::stof(std::string(pdecaySpeed->GetText()));

            tinyxml2::XMLElement* psmoothingDepth = pSpectre->FirstChildElement("smoothingDepth");
            spectre.smoothingDepth = std::stoi(std::string(psmoothingDepth->GetText()));

            tinyxml2::XMLElement* pspectreCorrectionDb = pSpectre->FirstChildElement("spectreCorrectionDb");
            spectre.spectreCorrectionDb = std::stoi(std::string(pspectreCorrectionDb->GetText()));
            
        }

        tinyxml2::XMLElement* pColorTheme = pRootElement->FirstChildElement("ColorTheme");
        if (NULL != pColorTheme) {
            tinyxml2::XMLElement* pspectreFillColor = pColorTheme->FirstChildElement("spectreFillColor");
            //std::stoll (std::string(pspectreFillColor->GetText()));
            colorTheme.spectreFillColor = std::stoll(std::string(pspectreFillColor->GetText()));

            tinyxml2::XMLElement* pspectreProfileColor = pColorTheme->FirstChildElement("spectreProfileColor");
            colorTheme.spectreProfileColor = std::stoll(std::string(pspectreProfileColor->GetText()));

            tinyxml2::XMLElement* pmainBGColor = pColorTheme->FirstChildElement("mainBGColor");
            colorTheme.mainBGColor = std::stoll(std::string(pmainBGColor->GetText()));

            tinyxml2::XMLElement* pwindowsBGColor = pColorTheme->FirstChildElement("windowsBGColor");
            colorTheme.windowsBGColor = std::stoll(std::string(pwindowsBGColor->GetText()));

            tinyxml2::XMLElement* pwindowsTitleBGColor = pColorTheme->FirstChildElement("windowsTitleBGColor");
            colorTheme.windowsTitleBGColor = std::stoll(std::string(pwindowsTitleBGColor->GetText()));

            tinyxml2::XMLElement* preceiveRegionColor = pColorTheme->FirstChildElement("receiveRegionColor");
            colorTheme.receiveRegionColor = std::stoll(std::string(preceiveRegionColor->GetText()));
        }
    } else {
        printf("Config file not found!\r\n");
    }
}

void Config::loadMemory() {
    memoryVector.clear();
    tinyxml2::XMLDocument doc;

    doc.LoadFile(CONFIG_FILENAME);

    tinyxml2::XMLElement* pRootElement = doc.RootElement();

    if (NULL != pRootElement) {
        tinyxml2::XMLElement* pMemory = pRootElement->FirstChildElement("Memory");
        if (NULL != pMemory) {
            //printf("pMemory found\n");
            //int count = 0;
            auto precord = pMemory->FirstChildElement("record");
            if (precord != NULL) {
                while (precord != NULL) {
                    auto pdesc = precord->FirstChildElement("desc");
                    auto pfreq = precord->FirstChildElement("freq");
                    auto pmodulation = precord->FirstChildElement("modulation");
                    auto pfilterWidth = precord->FirstChildElement("filterWidth");
                    memoryVector.push_back(
                        MemoryRecord{
                            pdesc->GetText(),
                            std::stof(pfreq->GetText()),
                            std::stoi(pmodulation->GetText()),
                            std::stoi(pfilterWidth->GetText())
                        }
                    );
                    //printf("%s %f %d %d\n", memoryVector[count].desc.c_str(), memoryVector[count].freq, memoryVector[count].modulation, memoryVector[count].filterWidth);
                    //count++;
                    precord = precord->NextSiblingElement();
                }
            }
        }
    } else {
        printf("Config file not found!\r\n");
    }
}
void Config::saveMemory() {
    tinyxml2::XMLDocument doc;

    doc.LoadFile(CONFIG_FILENAME);

    auto pRootElement = doc.RootElement();

    if (NULL != pRootElement) {
        auto pMemory = pRootElement->FirstChildElement("Memory");
        if (NULL != pMemory) {
            pMemory->DeleteChildren();

            for (int i = 0; i < memoryVector.size(); i++) {
                auto precord = pMemory->InsertNewChildElement("record");

                auto pdesc = precord->InsertNewChildElement("desc");
                pdesc->SetText(memoryVector[i].desc.c_str());

                auto pfreq = precord->InsertNewChildElement("freq");
                pfreq->SetText(std::string(std::to_string((int)memoryVector[i].freq)).c_str());

                auto pmodulation = precord->InsertNewChildElement("modulation");
                pmodulation->SetText(std::string(std::to_string(memoryVector[i].modulation)).c_str());

                auto pfilterWidth = precord->InsertNewChildElement("filterWidth");
                pfilterWidth->SetText(std::string(std::to_string(memoryVector[i].filterWidth)).c_str());
            }
        }
        doc.SaveFile(CONFIG_FILENAME);
    } else {
        printf("Config file not found!\r\n");
    }
}

void Config::save() {
    tinyxml2::XMLDocument doc;

    doc.LoadFile(CONFIG_FILENAME);

    tinyxml2::XMLElement* pRootElement = doc.RootElement();

    if (NULL != pRootElement) {

        tinyxml2::XMLElement* pApp = pRootElement->FirstChildElement("App");
        if (NULL != pApp) {
            tinyxml2::XMLElement* pwindowWidth = pApp->FirstChildElement("windowWidth");
            pwindowWidth->SetText(app.winWidth);

            tinyxml2::XMLElement* pwindowHeight = pApp->FirstChildElement("windowHeight");
            pwindowHeight->SetText(app.winHeight);
        }

        tinyxml2::XMLElement* pDevice = pRootElement->FirstChildElement("Device");
        if (NULL != pDevice) {

            tinyxml2::XMLElement* ptype = pDevice->FirstChildElement("type");
            if (delayedDeviceType == DeviceType::RSP) ptype->SetText(0);
            else if (delayedDeviceType == DeviceType::HACKRF) ptype->SetText(1);
            else if (delayedDeviceType == DeviceType::RTL) ptype->SetText(2);
            else ptype->SetText(0);

            tinyxml2::XMLElement* psamplingRateDiv = pDevice->FirstChildElement("decimation");
            psamplingRateDiv->SetText(delayedInputSamplerateDivider);

            tinyxml2::XMLElement* pHackRF = pDevice->FirstChildElement("HackRF");
            if (NULL != pHackRF) {
                tinyxml2::XMLElement* pbasebandFilter = pHackRF->FirstChildElement("basebandFilter");
                pbasebandFilter->SetText(hackrf.basebandFilter);

                tinyxml2::XMLElement* pdeviceSamplingRate = pHackRF->FirstChildElement("deviceSamplingRate");
                pdeviceSamplingRate->SetText(hackrf.deviceSamplingRate);

                tinyxml2::XMLElement* prxAmp = pHackRF->FirstChildElement("rxAmp");
                prxAmp->SetText(hackrf.rxAmp);

                tinyxml2::XMLElement* plnaGain = pHackRF->FirstChildElement("lnaGain");
                plnaGain->SetText(hackrf.lnaGain);

                tinyxml2::XMLElement* pvgaGain = pHackRF->FirstChildElement("vgaGain");
                pvgaGain->SetText(hackrf.vgaGain);

                //tinyxml2::XMLElement* ptxAmp = pHackRF->FirstChildElement("txAmp");
            }

            tinyxml2::XMLElement* pRSP = pDevice->FirstChildElement("RSP");
            if (NULL != pRSP) {
                tinyxml2::XMLElement* pbasebandFilter = pRSP->FirstChildElement("basebandFilter");
                pbasebandFilter->SetText(rsp.basebandFilter);

                tinyxml2::XMLElement* pdeviceSamplingRate = pRSP->FirstChildElement("deviceSamplingRate");
                pdeviceSamplingRate->SetText(rsp.deviceSamplingRate);

                tinyxml2::XMLElement* pgain = pRSP->FirstChildElement("gain");
                pgain->SetText(rsp.gain);

                tinyxml2::XMLElement* plna = pRSP->FirstChildElement("disableLna");
                plna->SetText(rsp.lna);

                tinyxml2::XMLElement* pdeviceDecimationFactor = pRSP->FirstChildElement("deviceDecimationFactor");
                pdeviceDecimationFactor->SetText(rsp.deviceDecimationFactor);

                tinyxml2::XMLElement* papi = pRSP->FirstChildElement("api");
                papi->SetText(rsp.api);
            }

            tinyxml2::XMLElement* pRTL = pDevice->FirstChildElement("RTL");
            if (NULL != pRTL) {
                tinyxml2::XMLElement* pdeviceSamplingRate = pRTL->FirstChildElement("deviceSamplingRate");
                pdeviceSamplingRate->SetText(rtl.deviceSamplingRate);

                tinyxml2::XMLElement* pgain = pRTL->FirstChildElement("gain");
                pgain->SetText(rtl.gain);
            }
        }

        tinyxml2::XMLElement* pReceiver = pRootElement->FirstChildElement("Receiver");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pstartFreq = pReceiver->FirstChildElement("startFreq");
            pstartFreq->SetText(startFrequency);

            tinyxml2::XMLElement* plastSelectedFreq = pReceiver->FirstChildElement("lastSelectedFreq");
            plastSelectedFreq->SetText(lastSelectedFreq);

            tinyxml2::XMLElement* pvolume = pReceiver->FirstChildElement("volume");
            pvolume->SetText(volume);

            tinyxml2::XMLElement* pfilterWidth = pReceiver->FirstChildElement("filterWidth");
            pfilterWidth->SetText(filterWidth);

            tinyxml2::XMLElement* pmodulation = pReceiver->FirstChildElement("modulation");
            pmodulation->SetText(receiver.modulation);

            tinyxml2::XMLElement* penableFrequencyShift = pReceiver->FirstChildElement("enableFrequencyShift");
            penableFrequencyShift->SetText((receiver.enableFrequencyShift == true) ? "1": "0");

            tinyxml2::XMLElement* pfrequencyShift = pReceiver->FirstChildElement("frequencyShift");
            pfrequencyShift->SetText(receiver.frequencyShift);

            tinyxml2::XMLElement* pAgc = pReceiver->FirstChildElement("Agc");
            if (NULL != pAgc) {
                tinyxml2::XMLElement* pthreshold = pAgc->FirstChildElement("threshold");
                pthreshold->SetText(receiver.agc.threshold);

                tinyxml2::XMLElement* patackSpeedMs = pAgc->FirstChildElement("atackSpeedMs");
                patackSpeedMs->SetText(receiver.agc.atackSpeedMs);

                tinyxml2::XMLElement* pholdingTimeMs = pAgc->FirstChildElement("holdingTimeMs");
                pholdingTimeMs->SetText(receiver.agc.holdingTimeMs);

                tinyxml2::XMLElement* preleaseSpeed = pAgc->FirstChildElement("releaseSpeed");
                preleaseSpeed->SetText(receiver.agc.releaseSpeed);

                tinyxml2::XMLElement* plastAmp = pAgc->FirstChildElement("lastAmp");
                plastAmp->SetText(receiver.agc.lastAmp);
            }
        }

        tinyxml2::XMLElement* pWaterfall = pRootElement->FirstChildElement("Waterfall");
        if (NULL != pWaterfall) {
            tinyxml2::XMLElement* pmin = pWaterfall->FirstChildElement("min");
            pmin->SetText(waterfallMin);

            tinyxml2::XMLElement* pmax = pWaterfall->FirstChildElement("max");
            pmax->SetText(waterfallMax);

            tinyxml2::XMLElement* pspeed = pWaterfall->FirstChildElement("speed");
            pspeed->SetText(waterfall.speed);
        }

        tinyxml2::XMLElement* pSpectre = pRootElement->FirstChildElement("Spectre");
        if (NULL != pSpectre) {
            tinyxml2::XMLElement* pratio = pSpectre->FirstChildElement("ratio");
            pratio->SetText(spectreRatio);

            tinyxml2::XMLElement* pspectreMin = pSpectre->FirstChildElement("min");
            pspectreMin->SetText(spectreMin);

            tinyxml2::XMLElement* pspeed = pSpectre->FirstChildElement("speed");
            pspeed->SetText(spectre.spectreSpeed);

            tinyxml2::XMLElement* pspeed2 = pSpectre->FirstChildElement("speed2");
            pspeed2->SetText(spectre.spectreSpeed2);

            tinyxml2::XMLElement* pstartBin = pSpectre->FirstChildElement("startBin");
            pstartBin->SetText(startBin);

            tinyxml2::XMLElement* pstopBin = pSpectre->FirstChildElement("stopBin");
            pstopBin->SetText(stopBin);

            tinyxml2::XMLElement* pvisibleStartFreq = pSpectre->FirstChildElement("visibleStartFreq");
            pvisibleStartFreq->SetText(spectre.visibleStartFreq);

            tinyxml2::XMLElement* pvisibleStopFreq = pSpectre->FirstChildElement("visibleStopFreq");
            pvisibleStopFreq->SetText(spectre.visibleStopFreq);

            tinyxml2::XMLElement* premoveDCBias = pSpectre->FirstChildElement("removeDCBias");
            premoveDCBias->SetText(removeDCBias);

            tinyxml2::XMLElement* pfftLen = pSpectre->FirstChildElement("fftLen");
            pfftLen->SetText(delayedFFTLen);

            tinyxml2::XMLElement* pstyle = pSpectre->FirstChildElement("style");
            pstyle->SetText(spectre.style);

            tinyxml2::XMLElement* pcontourShowsPower = pSpectre->FirstChildElement("contourShowsPower");
            pcontourShowsPower->SetText((spectre.contourShowsPower == true) ? "1" : "0");

            tinyxml2::XMLElement* pbottomCoeff = pSpectre->FirstChildElement("bottomCoeff");
            pbottomCoeff->SetText(spectre.bottomCoeff);

            tinyxml2::XMLElement* ptopCoeff = pSpectre->FirstChildElement("topCoeff");
            ptopCoeff->SetText(spectre.topCoeff);

            tinyxml2::XMLElement* psmoothing = pSpectre->FirstChildElement("hangAndDecay");
            psmoothing->SetText((spectre.hangAndDecay == true) ? "1" : "0");

            tinyxml2::XMLElement* pdecaySpeedDelta = pSpectre->FirstChildElement("decaySpeedDelta");
            pdecaySpeedDelta->SetText(spectre.decaySpeedDelta);

            tinyxml2::XMLElement* pdecaySpeed = pSpectre->FirstChildElement("decaySpeed");
            pdecaySpeed->SetText(spectre.decaySpeed);

            tinyxml2::XMLElement* psmoothingDepth = pSpectre->FirstChildElement("smoothingDepth");
            psmoothingDepth->SetText(spectre.smoothingDepth);

            tinyxml2::XMLElement* pspectreCorrectionDb = pSpectre->FirstChildElement("spectreCorrectionDb");
            pspectreCorrectionDb->SetText(spectre.spectreCorrectionDb);
        }

        tinyxml2::XMLElement* pColorTheme = pRootElement->FirstChildElement("ColorTheme");
        if (NULL != pColorTheme) {
            tinyxml2::XMLElement* pspectreFillColor = pColorTheme->FirstChildElement("spectreFillColor");
            pspectreFillColor->SetText(colorTheme.spectreFillColor);

            tinyxml2::XMLElement* pspectreProfileColor = pColorTheme->FirstChildElement("spectreProfileColor");
            pspectreProfileColor->SetText(colorTheme.spectreProfileColor);

            tinyxml2::XMLElement* pmainBGColor = pColorTheme->FirstChildElement("mainBGColor");
            pmainBGColor->SetText(colorTheme.mainBGColor);

            tinyxml2::XMLElement* pwindowsBGColor = pColorTheme->FirstChildElement("windowsBGColor");
            pwindowsBGColor->SetText(colorTheme.windowsBGColor);

            tinyxml2::XMLElement* pwindowsTitleBGColor = pColorTheme->FirstChildElement("windowsTitleBGColor");
            pwindowsTitleBGColor->SetText(colorTheme.windowsTitleBGColor);

            tinyxml2::XMLElement* preceiveRegionColor = pColorTheme->FirstChildElement("receiveRegionColor");
            preceiveRegionColor->SetText(colorTheme.receiveRegionColor);
        }

        doc.SaveFile(CONFIG_FILENAME);
        printf("Settings saved!\r\n");
    }
    else {
        printf("Config file not found!\r\n");
    }
}

void Config::setDevice(int deviceID) {
    switch (deviceID) {
        case 0:
            delayedDeviceType = DeviceType::RSP;
            break;
        case 1:
            delayedDeviceType = DeviceType::HACKRF;
            break;
        case 2:
            delayedDeviceType = DeviceType::RTL;
            break;
        default:
            delayedDeviceType = DeviceType::RSP;
    }
}

void Config::storeRecord(MemoryRecord memRec) {
    memoryVector.push_back(memRec);
    saveMemory();
    loadMemory();
}

void Config::editRecord(MemoryRecord correctedMemRec, int index) {
    memoryVector[index] = correctedMemRec;
    saveMemory();
    loadMemory();
}

void Config::deleteRecord(int index) {
    memoryVector.erase(std::next(std::begin(memoryVector), index));
    saveMemory();
    loadMemory();
}

void Config::calcOutputSamplerate() {
	int sampleRate = inputSamplerate;
	int div = 1;

	while (true) {
		div *= 2;
		if (sampleRate % div != 0) break;
		else {
			if (sampleRate / div <= 48000) break;
		}
	}
	outputSamplerateDivider = div;
	outputSamplerate = sampleRate / div;
	printf("Audio samplerate: %d, div: %d\r\n", outputSamplerate, outputSamplerateDivider);
}