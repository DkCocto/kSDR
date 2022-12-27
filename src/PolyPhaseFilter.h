#pragma once

#include "vector"
#include "FirFilter.h"
#include "Filter.h"

class PolyPhaseFilter {
	double* coeffs;
	double alpha = 1.0;
	std::vector<FirFilter>* subFilters;

private:

	void initSubFilters(double* coeffs, int coeffsLen, int R);

public:

	/**
		* Taps LEN must be an integer number of decimationRate
		* @param sampleRate
		* @param freq
		* @param decimationRate
		* @param len
		*/
	PolyPhaseFilter(double sampleRate, double freq, int decimationRate, int len);

	/**
		* Each time we get an input we rotate though the coefficients based on the decimate rate R and the position P.
		* We pass in enough coefficients to perform a complete rotation, because that produces one output value. It
		* is up to the calling routine to store the values and pass them in as a set.
		* @param in
		* @return
		*/
	double filter(double* in, int inLen);
};