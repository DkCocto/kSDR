#pragma once

#include "Env.h"
#include "fftw/fftw3.h"

#define REAL 0
#define IMAG 1

class HilbertTransformFFTW {

private:
	int len = 0;

	fftw_plan planForward;
	fftw_plan planBackward;

	fftw_complex* out;

public:

	HilbertTransformFFTW(int len);
	~HilbertTransformFFTW();
	fftw_complex* process(float* in);
	void resetOut();
};