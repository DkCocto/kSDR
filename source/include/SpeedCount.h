#pragma once

#include <cstdint>

#include "Average.h"
#include "chrono"

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