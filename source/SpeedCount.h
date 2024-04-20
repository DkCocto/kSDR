#pragma once

#include "chrono"
#include "Average.h"
#include <cstdint>

class SpeedCount {

	Average average;

	std::chrono::steady_clock::time_point begin;

public:

	SpeedCount();

	void start();
	uint64_t stop();
	float stopSmoothed();
	uint64_t stopAndPrintf();

};