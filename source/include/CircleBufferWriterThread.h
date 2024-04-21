#pragma once

#if defined(__linux__)
#else /* Windows*/
#include <windows.h>
#endif

#include <thread>

#include "CircleBufferNew.h"
#include "SoundCard.h"
#include "device/DeviceController.h"

#include "MyThread.h"

class CircleBufferWriterThread : public MyThread {

  CircleBufferNew<float> *soundWriterCircleBuffer;
  SoundCard *soundCard;

  int len;

  Config *config;
  DeviceController *deviceController;

  float *data;

public:
  CircleBufferWriterThread(Config *config, DeviceController *deviceController,
                           CircleBufferNew<float> *cb, SoundCard *sc);
  ~CircleBufferWriterThread();
  void run();
  std::thread start();
};
