#include "Config.h"

Config::Config() {
    load();

	inputChannelNumber							= 1;
	outputChannelNumber							= 1;

    switch (deviceType) {
        case RSP:
            break;
        case HACKRF:
            inputSamplerate = hackrf.deviceSamplingRate / inputSamplerateDivider;
            break;
        default:
            inputSamplerate = 4000000;
    }

	calcOutputSamplerate();

	//(fftLen / 2) / outputSamplerateDivider;
	bufferWriteAudioLen							= (outputSamplerateDivider * 2) * 2;

	//readSoundProcessorBufferLen				= (outputSamplerateDivider * 2) * 512;
	readSoundProcessorBufferLen					= fftLen; //fftLen

	audioReadFrameLen							= (outputSamplerateDivider * 2) * 32;
	audioWriteFrameLen							= (outputSamplerateDivider * 2) * 32;
	//readSize									= audioReadFrameLen * inputChannelNumber;

	circleBufferLen								= 2 * inputSamplerate;

	hilbertTransformLen							= 255;
	polyphaseFilterLen							= 256;

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
                default:
                    deviceType = HACKRF;
            }
        
            tinyxml2::XMLElement* psamplingRateDiv = pDevice->FirstChildElement("samplingRateDiv");
            inputSamplerateDivider = std::stoi(std::string(psamplingRateDiv->GetText()));

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
        }

        tinyxml2::XMLElement* pReceiver = pRootElement->FirstChildElement("Receiver");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pstartFreq = pReceiver->FirstChildElement("startFreq");
            startFrequency = std::stof(std::string(pstartFreq->GetText()));

            tinyxml2::XMLElement* plastSelectedFreq = pReceiver->FirstChildElement("lastSelectedFreq");
            lastSelectedFreq = std::stof(std::string(plastSelectedFreq->GetText()));

            tinyxml2::XMLElement* pvolume = pReceiver->FirstChildElement("volume");
            volume = std::stof(std::string(pvolume->GetText()));
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
            fftLen = std::stoi(std::string(pfftLen->GetText())) * 1024;
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
        tinyxml2::XMLElement* pReceiver = pRootElement->FirstChildElement("Receiver");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pstartFreq = pReceiver->FirstChildElement("startFreq");
            pstartFreq->SetText(startFrequency);

            tinyxml2::XMLElement* plastSelectedFreq = pReceiver->FirstChildElement("lastSelectedFreq");
            plastSelectedFreq->SetText(lastSelectedFreq);

            tinyxml2::XMLElement* pvolume = pReceiver->FirstChildElement("volume");
            pvolume->SetText(volume);
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
        }

        doc.SaveFile(CONFIG_FILENAME);
    }
    else {
        printf("Config file not found!");
    }
}

void Config::calcOutputSamplerate() {
	int sampleRate = inputSamplerate; // 2 000 000
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