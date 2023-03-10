#include "SpeedCount.h"

SpeedCount::SpeedCount() {
	average = Average(100);
}

void SpeedCount::start() {
	begin = std::chrono::steady_clock::now();
}

__int64 SpeedCount::stop() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

	return elapsed_nanos.count();
}

float SpeedCount::stopSmoothed() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	average.process((double)elapsed_ms.count());
	return (float)average.getAvg();
}

__int64 SpeedCount::stopAndPrintf() {
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

	printf("Time elapsed (micros): %d\r\n", (int)elapsed_ms.count());

	return elapsed_ms.count();
}
