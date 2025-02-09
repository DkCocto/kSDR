#include "SoundCard.h"

void SoundCard::init() {
	err = Pa_Initialize();

	inputParameters.device = Pa_GetDefaultInputDevice();

	if (inputParameters.device != paNoDevice) {
		if (config->deviceType != DeviceType::SOUNDCARD) inputParameters.channelCount = config->inputChannelNumber;
		else inputParameters.channelCount = config->soundcardDevice.inputChannelCount;
		inputParameters.sampleFormat = PA_SAMPLE_TYPE;
		inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
		inputParameters.hostApiSpecificStreamInfo = NULL;
	}

	outputParameters.device = Pa_GetDefaultOutputDevice();

	if (outputParameters.device != paNoDevice) {
		outputParameters.channelCount = config->outputChannelNumber;
		outputParameters.sampleFormat = PA_SAMPLE_TYPE;
		outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;
	}
}

void SoundCard::showError(PaError err) {
	printf("An error occurred while using the portaudio stream\n");
	printf("Error number: %d\n", err);
	printf("Error message: %s\n", Pa_GetErrorText(err));
}

SoundCard::SoundCard(Config* config) {
	this->config = config;
	init();
}

SoundCard::~SoundCard() {
	close();
}

void SoundCard::open() {
	if (inputParameters.device != paNoDevice) {
		//Sound input stream
		int samplingRate = (config->deviceType != SOUNDCARD) ? config->inputSamplerateSound : config->soundcardDevice.deviceSamplingRate;
		int frameCount = (config->deviceType != SOUNDCARD) ? config->audioReadFrameLen : config->soundcardDevice.inputFrameCount;
		err = Pa_OpenStream(
			&inputStream,
			&inputParameters,
			NULL,
			samplingRate,
			frameCount,
			paClipOff,
			NULL,
			NULL);
		if (err != paNoError) showError(err);
	}

	//err = Pa_StartStream(inputStream);
	//if (err != paNoError) throw [&]() { showError(err); };
	//------------------

	if (outputParameters.device != paNoDevice) {

		//Sound output stream
		err = Pa_OpenStream(
			&outputStream,
			NULL,
			&outputParameters,
			config->outputSamplerate,
			config->audioWriteFrameLen,
			paClipOff,
			NULL,
			NULL);
		if (err != paNoError) showError(err);

		err = Pa_StartStream(outputStream);
		if (err != paNoError) showError(err);
		//------------------

	}
}

void SoundCard::close() {
	if (inputStream) {
		Pa_AbortStream(inputStream);
		Pa_CloseStream(inputStream);
	}
	if (outputStream) {
		Pa_AbortStream(outputStream);
		Pa_CloseStream(outputStream);
	}
	Pa_Terminate();
	printf("SoundCard object destructed!\r\n");
}

float* SoundCard::read(float* buffer, int len) {
	if (inputStream && buffer != nullptr) {
		err = Pa_ReadStream(inputStream, buffer, len);
		if (err != paNoError) printf("Error message : % s\n", Pa_GetErrorText(err));
	} else return nullptr;
	return buffer;
}

void SoundCard::write(float* buffer, int len) {
	if (outputStream) {
		err = Pa_WriteStream(outputStream, buffer, len);
		if (err != paNoError) {
			printf("Error message : % s\n", Pa_GetErrorText(err));
			//throw printf("Error message : % s\n", Pa_GetErrorText(err));
		}
	}
}

void SoundCard::stopInput() {
	if (inputStream) {
		Pa_AbortStream(inputStream);
	}
}

void SoundCard::startInput() {
	if (inputStream) {
		err = Pa_StartStream(inputStream);
		if (err != paNoError) throw [&]() { showError(err); };
	}
}

int SoundCard::availableToRead() {
	if (inputStream) return Pa_GetStreamReadAvailable(inputStream);
	else {
		throw printf("Stream is empty!");
		return -1;
	}
}

bool SoundCard::isInputAvailable() {
	return inputParameters.device != paNoDevice;
}

bool SoundCard::isOutputAvailable() {
	return outputParameters.device != paNoDevice;
}

PaStream* SoundCard::getInputStream() {
	return inputStream;
}
