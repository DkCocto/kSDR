#include "Config.h"

Config::Config(int inputSamplerate, int inputSamplerateDivider, int outputSamplerateDivider) {
	inputChannelNumber							= 1;
	outputChannelNumber							= 1;
	this->inputSamplerate						= inputSamplerate;
	this->inputSamplerateDivider				= inputSamplerateDivider;
	this->outputSamplerateDivider				= outputSamplerateDivider;

	outputSamplerate							= inputSamplerate / outputSamplerateDivider;

	bufferWriteAudioLen							= (outputSamplerateDivider * 2) * 4;
	readSoundProcessorBufferLen					= (outputSamplerateDivider * 2) * 32;

	audioReadFrameLen							= (outputSamplerateDivider * 2) * 32;
	audioWriteFrameLen							= (outputSamplerateDivider * 2) * 32;
	readSize									= audioReadFrameLen * inputChannelNumber;

	circleBufferLen								= 8 * 1024 * 1024;

	hilbertTransformLen							= 63;
	polyphaseFilterLen							= 512;

	fftLen										= 4096;

	fftBandwidth								= (float)inputSamplerate / (float)fftLen;

	startFrequency								= 7100000;
}