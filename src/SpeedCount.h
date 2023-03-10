#pragma once

#include "chrono"
#include "KalmanFilter.h"
#include "Average.h"

class SpeedCount {

	Average average;

	std::chrono::steady_clock::time_point begin;

public:

	SpeedCount();

	void start();
	__int64 stop();
	float stopSmoothed();
	__int64 stopAndPrintf();

};