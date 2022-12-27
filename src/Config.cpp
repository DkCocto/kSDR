#include "Config.h"
#include "stdio.h"
#include "tinyxml2/tinyxml2.h"
#include "string"

Config::Config(int inputSamplerate) {
	inputChannelNumber							= 1;
	outputChannelNumber							= 1;
	this->inputSamplerate						= inputSamplerate;

	inputSamplerateDivider						= 2;

	calcOutputSamplerate();

	fftLen										= 32 * 1024;

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

    load();
}

Config::~Config() {
    save();
}

//Loading config from config file
void Config::load() {

    tinyxml2::XMLDocument doc;

    doc.LoadFile(CONFIG_FILENAME);

    tinyxml2::XMLElement* pRootElement = doc.RootElement();

    if (NULL != pRootElement) {

        tinyxml2::XMLElement* pReceiver = pRootElement->FirstChildElement("Receiver");
        if (NULL != pReceiver) {
            tinyxml2::XMLElement* pstartFreq = pReceiver->FirstChildElement("startFreq");
            startFrequency = std::stof(std::string(pstartFreq->GetText()));
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
			if (sampleRate / div <= 192000) break;
		}
	}
	outputSamplerateDivider = div;
	outputSamplerate = sampleRate / div;
	printf("Audio samplerate: %d, div: %d\r\n", outputSamplerate, outputSamplerateDivider);
}
