#pragma once

#include "AGC.h"
#include "CircleBufferNew.h"
#include "Config.h"
#include "Delay.h"
#include "FIR.h"
#include "FMDemodulator.h"
#include "HilbertTransform.h"
#include "Mixer.h"
#include "PolyPhaseFilter.h"
#include "ReceiverLogic.h"
#include "device/DeviceController.h"
#include "MyThread.h"

class SoundProcessorThread : public MyThread {

  float *outputData = nullptr;

  Mixer mixer;

  HilbertTransform *hilbertTransform;
  Delay *delay;

  PolyPhaseFilter firFilterI;
  PolyPhaseFilter firFilterQ;

  float *decimateBufferI;
  float *decimateBufferQ;

  CircleBufferNew<float> *soundWriterCircleBuffer;
  SpectreHandler *specHandler;

  Config *config;

  FIR fir;
  FIR firI;
  FIR firQ;

  DeviceController *devCnt;

  ViewModel *viewModel;

  ReceiverLogic *receiverLogic;

  AGC agc;

  FMDemodulator fmDemodulator;

  DCRemove dcRemover;

  template <typename DEVICE, typename DATATYPE>
  void initProcess(DEVICE *device);

public:
  int len; // ������ ���������� �� 1 ��� �� ��������� ������

  SoundProcessorThread(DeviceController *devCnt, ViewModel *viewModel,
                       ReceiverLogic *receiverLogic, Config *config,
                       CircleBufferNew<float> *soundWriterCircleBuffer,
                       SpectreHandler *specHandler);
  ~SoundProcessorThread();

  void initFilters(int filterWidth);

  template <typename T, typename D> void processData(T *data, D *device);

  void run();

  std::thread start();
};