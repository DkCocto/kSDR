#include "SoundCard.h"

void SoundCard::init() {
	err = Pa_Initialize();

	/*inputParameters.device = Pa_GetDefaultInputDevice();

	inputParameters.channelCount = config->inputChannelNumber;
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;*/

	outputParameters.device = Pa_GetDefaultOutputDevice();

	outputParameters.channelCount = environment->getConfig()->outputChannelNumber;
	outputParameters.sampleFormat = PA_SAMPLE_TYPE;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
}

void SoundCard::showError(PaError err) {
	printf("An error occurred while using the portaudio stream\n");
	printf("Error number: %d\n", err);
	printf("Error message: %s\n", Pa_GetErrorText(err));
}

SoundCard::SoundCard(Environment* environment) {
	this->environment = environment;
	init();
}

SoundCard::~SoundCard() {
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

void SoundCard::open() {
	////Sound input stream
	//err = Pa_OpenStream(
	//	&inputStream,
	//	&inputParameters,
	//	NULL,
	//	INPUT_SAMPLE_RATE,
	//	FRAMES_PER_BUFFER,
	//	paClipOff,
	//	NULL,
	//	NULL);
	//if (err != paNoError) throw [&]() { showError(err); };

	//err = Pa_StartStream(inputStream);
	//if (err != paNoError) throw [&]() { showError(err); };
	////------------------


	//Sound output stream
	err = Pa_OpenStream(
		&outputStream,
		NULL,
		&outputParameters,
		environment->getConfig()->outputSamplerate,
		environment->getConfig()->audioWriteFrameLen,
		paClipOff,
		NULL,
		NULL);
	if (err != paNoError) throw [&]() { showError(err); };

	err = Pa_StartStream(outputStream);
	if (err != paNoError) throw [&]() { showError(err); };
	//------------------
}

float* SoundCard::read(float* buffer, int len) {
	if (inputStream) {
		err = Pa_ReadStream(inputStream, buffer, len);
		if (err != paNoError) throw printf("Error message : % s\n", Pa_GetErrorText(err));
	}
	else throw printf("Stream is empty!");
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
	else throw printf("Stream is empty!");
}