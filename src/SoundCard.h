#pragma once

#include "Env.h"
#include "portaudio/portaudio.h"
#include "Environment.h"

class SoundCard {

private:

	Environment* environment;

	PaStream* inputStream = NULL;
	PaStream* outputStream = NULL;

	PaError err;
	PaStreamParameters inputParameters, outputParameters;
	//float buffer[FRAMES_PER_BUFFER * NUM_INPUT_CHANNELS]; /* stereo input buffer */

	void init();
	void showError(PaError err);

public:

	SoundCard(Environment* environment);

	~SoundCard();

	void open();

	float* read(float* buffer, int len);

	void write(float* buffer, int len);

};