#pragma once

#include "ColoredSpectreBG.h"
#include "Environment.h"
#include "FlowingFFTSpectre.h"
#include "KalmanFilter.h"
#include "ReceiverLogic.h"
#include "ReceiverRegionInterface.h"
#include "SpectreWindowData.h"
#include "ViewModel.h"
#include "Waterfall.h"
#include <imgui.h>
#include "vector"

using namespace std;

class Spectre {

  ViewModel *viewModel;

  // unique_ptr<KalmanFilter> maxdBKalman;
  unique_ptr<KalmanFilter> ratioKalman;
  unique_ptr<KalmanFilter> spectreTranferKalman;

  Config *config;

  ColoredSpectreBG coloredSpectreBG;

  int spectreWidth = 0;
  int spectreHeight = 0;

  // bool disableControl_ = false;

  Environment *env;

public:
  struct MIN_MAX {
    float min;
    float max;
    float average;
  };

  struct Region {
    ImVec2 x1;
    ImVec2 x2;
  };

  void releaseControl();

  void disableControlForID(int id);
  void enableControlForID(int id);

  bool isMouseOnRegion(Spectre::Region region);

private:
  MIN_MAX minMax;

  SpectreWindowData sWD;

  struct WINDOW_FRAME {
    ImVec2 UPPER_RIGHT;
    ImVec2 BOTTOM_LEFT;
  };

  Spectre::MIN_MAX getMinMaxInSpectre(std::vector<float> spectreData, int len);

  void handleEvents(int spectreWidthInPX, ReceiverLogic *receiverLogic,
                    FlowingSpectre *flowingSpectre);

  int getMousePosXOnSpectreWindow();

  void drawFreqMarks(ImDrawList *draw_list, ImVec2 startWindowPoint,
                     ImVec2 windowLeftBottomCorner, int spectreWidthInPX,
                     int spectreHeight, FlowingSpectre *flowingSpec);

  void drawSpectreContour(FFTData::OUTPUT *fullSpectreData,
                          ImDrawList *draw_list, FlowingSpectre *flowingSpec,
                          SpectreHandler *specHandler);

  void drawFreqPointerMark(ImVec2 startWindowPoint,
                           ImVec2 windowLeftBottomCorner, int spectreWidthInPX,
                           ImDrawList *draw_list, ReceiverLogic *receiverLogic);

  void drawMemoryMarks(ImDrawList *draw_list, FlowingSpectre *flowingSpec,
                       ReceiverLogic *receiverLogic);

  unique_ptr<ReceiverRegionInterface> receiverRegionInterface;

  vector<int> disableControlQueue;

  bool isControlDisabled();

public:
  Spectre(Environment *env);

  void executeMemoryRecord(Config::MemoryRecord record,
                           ReceiverLogic *receiverLogic);

  unique_ptr<Waterfall> waterfall;

  MIN_MAX getMinMaxInSpectre();

  void draw();

  WINDOW_FRAME windowFrame{ImVec2(0, 0), ImVec2(0, 0)};

  void waterfallAutoColorCorrection();
  void spectreRatioAutoCorrection();

  vector<int> getDisableControlQueue();
};