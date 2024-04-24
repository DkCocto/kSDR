#pragma once

#include <imgui.h>

struct SpectreWindowData {
  const int rightPadding = 40;
  const int leftPadding = 40;
  const int waterfallPaddingTop = 50;
  ImVec2 startWindowPoint{0, 0};
  ImVec2 windowLeftBottomCorner{0, 0};
};