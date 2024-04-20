#pragma once

#if defined(__linux__)
#else /* Windows*/
#include <windows.h>
#endif

#include <thread>

#include "DCRemove.h"
#include "ViewModel.h"
#include "WindowBlackman.h"
#include "WindowBlackmanHarris.h"
#include "fftw3.h"
#include "mutex"

#include "FFTData.h"
#include "MyThread.h"

#include "device/DeviceController.h"

class SpectreHandler : public MyThread {

private:
  float *tmpArray;
  float *tmpArray2;

  // audiofft::AudioFFT fft;

  Config *config;

  // »нициализируем оконный массив с размерностью длины массива буфера спектра
  // (возвращаетс¤ массив длиной +1)
  WindowBlackman *wb;
  WindowBlackmanHarris *wbh;

  int savedBufferPos = -1;

  int complexLen;

  float *realInput;
  float *imInput;

  float *realOut;
  float *imOut;

  float *superOutput;
  float *outputWaterfall;

  bool firstRun = true;

  int spectreSpeed = 30;

  int spectreSize = 0;

  bool busy = false;
  bool readyToProcess = false;
  bool outputting = false;

  fftw_plan fftwPlan;
  fftw_complex *inData;
  fftw_complex *outData;

  float *speedDelta;

  FFTData *fftData;

  std::mutex spectreDataMutex;

  std::atomic_bool ready = false;

  DCRemove dcRemove;

  ViewModel *viewModel;

  DeviceController *deviceController;

  void run();
  void dataPostprocess();
  template <typename DEVICE, typename DATATYPE>
  void prepareToProcess(DEVICE *device);
  template <typename T, typename D> void processFFT(T *data, D *device);
  float psd(float re, float im);
  float average(float avg, float new_sample, int n);

public:
  SpectreHandler(Config *config, FFTData *fftData, ViewModel *viewModel,
                 DeviceController *deviceController);
  ~SpectreHandler();

  std::thread start();

  FFTData *getFFTData();
};