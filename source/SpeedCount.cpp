#include "SpeedCount.h"

#include <cstdio>

SpeedCount::SpeedCount() {
	average = Average(100);
}

void SpeedCount::start() {
	begin = std::chrono::steady_clock::now();
}

uint64_t SpeedCount::stop() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

	return elapsed_nanos.count();
}

float SpeedCount::stopSmoothed() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	average.process((double)elapsed_ms.count());
	return (float)average.getAvg();
}

uint64_t SpeedCount::stopAndPrintf() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

	printf("Time elapsed (nanoseconds): %d\r\n", (int)elapsed_ms.count());

	return elapsed_ms.count();
}
