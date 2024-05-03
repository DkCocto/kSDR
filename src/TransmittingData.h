#pragma once

#include "Env.h"
#include "ComplexOscillator.h"
#include "FIR.h"
#include "CosOscillator.h"
#include "Mixer.h"
#include "HilbertTransform.h"
#include "Delay.h"
#include "PolyPhaseFilter.h"
#include "Config.h"
#include "HilbertTransformFFTW.h"
#include "WindowBlackman.h"

class TransmittingData {

private:

	ComplexOscillator* carierSignal;
	ComplexOscillator* complexOscilator;

	FIR firI;
	FIR firQ;

	CosOscillator* sourceSignal;

	Mixer* mixer1;
	//Mixer* mixer2;

	HilbertTransform* hilbertTransform;

	HilbertTransform* hilbertTransform2;
	Delay* delay;
	Delay* delay2;

	PolyPhaseFilter firFilterI;

	Config* config;

	HilbertTransformFFTW* hilbertTransformFFTW;
	HilbertTransformFFTW* hilbertTransformFFTW1;

	WindowBlackman* windowBlackman;

public:

	TransmittingData(Config* config, int sampleRate, int freq);
	~TransmittingData();

	Signal* nextBuffer();
	void setFreq(int freq);
};