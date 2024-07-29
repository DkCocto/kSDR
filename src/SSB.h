#pragma once

#include "Config.h"
#include "Modulation.h"
#include "ComplexSignal.h"
#include "ComplexOscillator.h"
#include "Mixer.h"
//#include "WindowBlackmanHarris.h"
#include "SinOscillator.h"
#include "HilbertTransform.h"
#include "Delay.h"
#include "FastFir/jfastfir.h"
#include "FIR.h"

//#define SSB_HILBERT_TRANSFORM_LEN 128 * 1024

class SSBModulation : Modulation {

private:

	DataStruct* ssbSignalData;

	const int carierFreq = 6000;

	Config* config = nullptr;

	//create pointer
	JFastFIRFilter* fastfir;
	JFastFIRFilter* fastfir2;

	FIR audioFilter;

	HilbertTransform* hilbertTransform1;
	HilbertTransform* hilbertTransform2;

	Delay* delay1;
	Delay* delay2;

	ComplexOscillator carierSignal;

	Mixer mixer;

	int relation;
	int upsamplingDataLen;

	int inputDataLen;

	std::vector<double> upsamplingDataQ;
	std::vector<double> upsamplingDataI;

	std::vector<float> bufQ;
	std::vector<float> bufI;

	int storedFilterWidth = 0;
	int storedModulation = 0;

	void initFilters();

public:

	SSBModulation(Config* config, int inputDataLen);
	~SSBModulation();

	DataStruct* processData(float* data);
	
	void setFreq(int freq);

};