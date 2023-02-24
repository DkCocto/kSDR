#include "Config.h"

Config::Config() {
    load();

	inputChannelNumber							= 1;
	outputChannelNumber							= 1;

    switch (deviceType) {
        case RSP:
            inputSamplerate = (rsp.deviceSamplingRate / rsp.deviceDecimationFactor) / inputSamplerateDivider;
            break;
        case HACKRF:
            inputSamplerate = hackrf.deviceSamplingRate / inputSamplerateDivider;
            break;
        case RTL:
            inputSamplerate = rtl.deviceSamplingRate / inputSamplerateDivider;
            break;
        default:
            inputSamplerate = 4000000;
    }

	calcOutputSamplerate();

	bufferWriteAudioLen							= (outputSamplerateDivider * 2) * 2;

	readSoundProcessorBufferLen					= fftLen; //fftLen

	audioReadFrameLen							= (outputSamplerateDivider * 2) * 32;
	audioWriteFrameLen							= (outputSamplerateDivider * 2) * 32;

	circleBufferLen								= 4 * inputSamplerate;

	hilbertTransformLen							= 255;
	polyphaseFilterLen							= 128;

	fftBandwidth								= (float)inputSamplerate / (float)fftLen;
}

Config::~Config() {
    save();
    delete device;
}

//Loading config from config file
void Config::load() {

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
                    deviceType = RSP;
                    break;
                case 1:
                    deviceType = HACKRF;
                    break;
                case 2:
                    deviceType = RTL;
                    break;
                default:
                    deviceType = HACKRF;
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
        }

        tinyxml2::XMLElement* pWaterfall = pRootElement->FirstChildElement("Waterfall");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pmin = pWaterfall->FirstChildElement("min");
            waterfallMin = std::stof(std::string(pmin->GetText()));

            tinyxml2::XMLElement* pmax = pWaterfall->FirstChildElement("max");
            waterfallMax = std::stof(std::string(pmax->GetText()));
        }

        tinyxml2::XMLElement* pSpectre = pRootElement->FirstChildElement("Spectre");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pratio = pSpectre->FirstChildElement("ratio");
            spectreRatio = std::stof(std::string(pratio->GetText()));
                
            tinyxml2::XMLElement* pspectreMin = pSpectre->FirstChildElement("min");
            spectreMin = std::stof(std::string(pspectreMin->GetText()));

            tinyxml2::XMLElement* pspeed = pSpectre->FirstChildElement("speed");
            spectreSpeed = std::stof(std::string(pspeed->GetText()));

            tinyxml2::XMLElement* pfftLen = pSpectre->FirstChildElement("fftLen");
            fftLen = std::stoi(std::string(pfftLen->GetText()));
            delayedFFTLen = fftLen;

            tinyxml2::XMLElement* pstartBin = pSpectre->FirstChildElement("startBin");
            startBin = std::stoi(std::string(pstartBin->GetText()));

            tinyxml2::XMLElement* pstopBin = pSpectre->FirstChildElement("stopBin");
            stopBin = std::stoi(std::string(pstopBin->GetText()));

            //Проверка на случай если fftLen изменилось, а границы спектра выходят за общую границу FFT
            if (startBin > (fftLen / 2) || stopBin > (fftLen / 2)) {
                startBin = 0;
                stopBin = (fftLen / 2) - 1;
            }

            tinyxml2::XMLElement* premoveDCBias = pSpectre->FirstChildElement("removeDCBias");
            removeDCBias = std::stoi(std::string(premoveDCBias->GetText()));

            
        }
    } else {
        printf("Config file not found!");
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
            if (delayedDeviceType == RSP) ptype->SetText(0);
            else if (delayedDeviceType == HACKRF) ptype->SetText(1);
            else if (delayedDeviceType == RTL) ptype->SetText(2);
            else ptype->SetText(0);

            tinyxml2::XMLElement* psamplingRateDiv = pDevice->FirstChildElement("decimation");
            psamplingRateDiv->SetText(delayedInputSamplerateDivider);

            tinyxml2::XMLElement* pHackRF = pDevice->FirstChildElement("HackRF");
            if (NULL != pHackRF) {
                //tinyxml2::XMLElement* pbasebandFilter = pHackRF->FirstChildElement("basebandFilter");

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
        }

        tinyxml2::XMLElement* pWaterfall = pRootElement->FirstChildElement("Waterfall");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pmin = pWaterfall->FirstChildElement("min");
            pmin->SetText(waterfallMin);

            tinyxml2::XMLElement* pmax = pWaterfall->FirstChildElement("max");
            pmax->SetText(waterfallMax);
        }

        tinyxml2::XMLElement* pSpectre = pRootElement->FirstChildElement("Spectre");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pratio = pSpectre->FirstChildElement("ratio");
            pratio->SetText(spectreRatio);

            tinyxml2::XMLElement* pspectreMin = pSpectre->FirstChildElement("min");
            pspectreMin->SetText(spectreMin);

            tinyxml2::XMLElement* pspeed = pSpectre->FirstChildElement("speed");
            pspeed->SetText(spectreSpeed);

            tinyxml2::XMLElement* pstartBin = pSpectre->FirstChildElement("startBin");
            pstartBin->SetText(startBin);

            tinyxml2::XMLElement* pstopBin = pSpectre->FirstChildElement("stopBin");
            pstopBin->SetText(stopBin);

            tinyxml2::XMLElement* premoveDCBias = pSpectre->FirstChildElement("removeDCBias");
            premoveDCBias->SetText(removeDCBias);

            tinyxml2::XMLElement* pfftLen = pSpectre->FirstChildElement("fftLen");
            pfftLen->SetText(delayedFFTLen);
        }

        doc.SaveFile(CONFIG_FILENAME);
    }
    else {
        printf("Config file not found!");
    }
}

void Config::setDevice(int deviceID) {
    switch (deviceID) {
        case 0:
            delayedDeviceType = RSP;
            break;
        case 1:
            delayedDeviceType = HACKRF;
            break;
        case 2:
            delayedDeviceType = RTL;
            break;
        default:
            delayedDeviceType = RSP;
    }
}

void Config::calcOutputSamplerate() {
	int sampleRate = inputSamplerate;
	int div = 1;

	while (true) {
		div *= 2;
		if (sampleRate % div != 0) break;
		else {
			if (sampleRate / div <= 44000) break;
		}
	}
	outputSamplerateDivider = div;
	outputSamplerate = sampleRate / div;
	printf("Audio samplerate: %d, div: %d\r\n", outputSamplerate, outputSamplerateDivider);
}