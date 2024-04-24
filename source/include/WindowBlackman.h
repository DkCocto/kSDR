#pragma once

class WindowBlackman {

  int len = 0;
  float *weightArray;

public:
  WindowBlackman(int fftLen); // this->len = fftLen * 2;
  ~WindowBlackman();

  float *getWeights();

  void init();
};