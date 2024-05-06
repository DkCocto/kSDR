#pragma once

#include "Env.h"
#include "portaudio/portaudio.h"
#include "Config.h"

class SoundCard {

private:

	PaStream* inputStream = NULL;
	PaStream* outputStream = NULL;

	PaError err;
	PaStreamParameters inputParameters, outputParameters;
	//float buffer[FRAMES_PER_BUFFER * NUM_INPUT_CHANNELS]; /* stereo input buffer */

	void init();
	void showError(PaError err);

	Config* config;

public:

	SoundCard(Config* config);

	~SoundCard();

	void open();
	void close();
	float* read(float* buffer, int len);

	void write(float* buffer, int len);

	void stopInput();
	void startInput();

	int availableToRead();

};