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

	const int carierFreq = 6000;

	FIR audioFilter;

	DataStruct* signalData;

	Config* config = nullptr;

	float* amData;
	std::vector<double> upsampledData;

	std::vector<double> upsampledDataQ;
	std::vector<double> upsampledDataI;

	Mixer* mixer = nullptr;

	JFastFIRFilter* upsampledDataFilterI;
	JFastFIRFilter* upsampledDataFilterQ;

	CosOscillator* carierOscillator = nullptr;

	int inputDataLen = 0;

	int storedFilterWidth = 0;

	int relation;
	int upsamplingDataLen;

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