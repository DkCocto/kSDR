#pragma once

#include "Average.h"
#include "ReceiverLogic.h"
#include "ViewModel.h"
#include "FFTData.h"
#include <imgui.h>

class SMeter {

  int X, Y;
  int width, height;

  ViewModel *viewModel;

  void drawGrid(ImDrawList *draw_list);
  void drawLevel(ImDrawList *draw_list, FFTData::OUTPUT *spectreData,
                 ReceiverLogic *receiverLogic);

  float fromdBToLevel(float dBValue);

  float fromLevelToDb(float level);

  const char *getLevelDecodedString(float dBValue);

  Average averageSignalDb;

public:
  SMeter(ViewModel *viewModel);

  void update(int X, int Y, int width, int height);
  void draw(ImDrawList *draw_list, FFTData::OUTPUT *spectreData,
            ReceiverLogic *receiverLogic);

  float getSignaldB(FFTData::OUTPUT *spectreData, ReceiverLogic *receiverLogic);
};