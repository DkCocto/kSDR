#pragma once

#include "vector"

class FirFilter {

	std::vector<double> coeffs;
	double* xv;  // This array holds the delayed values
	int M; // The number of taps, the length of the filter
	double Fc = 0; // Will be set to cutoffFreq/SAMPLE_RATE; 

public:

	FirFilter();

	FirFilter(std::vector<double> taps, int tabsLen);

	/**
	 * Calculate the result with convolution.  This assumes that the filter kernel coeffs are already reversed as
	 * new valuea are added to the end of the delay line xv.  Note that a symmetrical filter such as a raised
	 * cosine does not need to be reversed.
	 * @param in
	 * @return
	 */
	double filter(double in);
};