#pragma once

#include "Env.h"
#include "portaudio/portaudio.h"
#include "Config.h"

class SoundCard {

private:

	Config* config;

	PaStream* inputStream = NULL;
	PaStream* outputStream = NULL;

	PaError err;
	PaStreamParameters inputParameters, outputParameters;
	//float buffer[FRAMES_PER_BUFFER * NUM_INPUT_CHANNELS]; /* stereo input buffer */

	void init();
	void showError(PaError err);

public:

	SoundCard(Config* config);

	~SoundCard();

	void open();

	float* read(float* buffer, int len);

	void write(float* buffer, int len);

};