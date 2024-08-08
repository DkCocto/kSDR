#pragma once

#include "Modulation.h"
#include "Mixer.h"
#include "CosOscillator.h"
#include "HilbertTransform.h"
#include "Delay.h"
#include "FastFir/jfastfir.h"
#include "FIR.h"

class AMModulation : Modulation {

private:

	FIR audioFilter;

	DataStruct* signalData;

	Config* config = nullptr;

	float* amData;
	std::vector<double> upsampledData;

	Mixer* mixer = nullptr;

	JFastFIRFilter* fastfir;
	JFastFIRFilter* fastfir2;

	CosOscillator* cosOscillator = nullptr;

	int inputDataLen = 0;

	int storedFilterWidth = 0;

	int relation;
	int upsamplingDataLen;

	std::vector<double> upsamplingDataQ;
	std::vector<double> upsamplingDataI;

	std::vector<float> amQ;
	std::vector<float> amI;

	HilbertTransform* hilbertTransform1;
	Delay* delay1;

	void initFilters();

	float max = 0;
	float min = 0;

public:

	AMModulation(Config* config, int inputDataLen);
	~AMModulation();

	DataStruct* processData(float* input);
	void setFreq(int freq);
};