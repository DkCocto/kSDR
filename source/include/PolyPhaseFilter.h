#pragma once

#include "vector"
#include "FirFilter.h"

class PolyPhaseFilter {
	std::vector<float> coeffs;
	float alpha = 0.5f;
	std::vector<FirFilter> subFilters;

private:

	void initSubFilters(std::vector<float> coeffs, int coeffsLen, int R);

public:

	/**
	* Taps LEN must be an integer number of decimationRate
	* @param sampleRate
	* @param freq
	* @param decimationRate
	* @param len
	*/
	void initCoeffs(float sampleRate, float freq, int decimationRate, int len);

	PolyPhaseFilter();

	/**
		* Each time we get an input we rotate though the coefficients based on the decimate rate R and the position P.
		* We pass in enough coefficients to perform a complete rotation, because that produces one output value. It
		* is up to the calling routine to store the values and pass them in as a set.
		* @param in
		* @return
		*/
	float filter(float* in, int inLen);
};