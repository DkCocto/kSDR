#include "Config.h"
#include "stdio.h"

Config::Config(int inputSamplerate) {
	inputChannelNumber							= 1;
	outputChannelNumber							= 1;
	this->inputSamplerate						= inputSamplerate;

	inputSamplerateDivider						= 1;

	calcOutputSamplerate();

	fftLen										= 32 * 1024;

	//(fftLen / 2) / outputSamplerateDivider;
	bufferWriteAudioLen							= (outputSamplerateDivider * 2) * 2;

	//readSoundProcessorBufferLen				= (outputSamplerateDivider * 2) * 512;
	readSoundProcessorBufferLen					= fftLen; //fftLen

	audioReadFrameLen							= (outputSamplerateDivider * 2) * 32;
	audioWriteFrameLen							= (outputSamplerateDivider * 2) * 32;
	//readSize									= audioReadFrameLen * inputChannelNumber;

	circleBufferLen								= inputSamplerate;

	hilbertTransformLen							= 127;
	polyphaseFilterLen							= 256;

	fftBandwidth								= (float)inputSamplerate / (float)fftLen;

	startFrequency								= 3600000;
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
