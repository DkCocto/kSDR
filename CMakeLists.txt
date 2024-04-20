cmake_minimum_required(VERSION 3.18)
project(ksdr)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(libraries)

add_executable(ksdr 
  source/AGC.cpp
  source/CircleBuffer.cpp
  source/CircleBufferReaderThread.cpp
  source/CircleBufferWriterThread.cpp
  source/ColoredSpectreBG.cpp
  source/ComplexOscillator.cpp
  source/ComplexSignal.cpp
  source/Config.cpp
  source/CosOscillator.cpp
  source/CPU.cpp
  source/DataReceiver.cpp
  source/DCRemove.cpp
  source/Delay.cpp
  source/Device.cpp
  source/device/DeviceController.cpp
  source/device/DeviceInterface.cpp
  source/device/DeviceN.cpp
  source/device/HackRFDevice.cpp
  source/device/HackRFInterface.cpp
  source/device/RSPDevice.cpp
  source/device/RSPInterface.cpp
  source/device/RTLDevice.cpp
  source/device/RTLInterface.cpp
  source/Display.cpp
  source/Environment.cpp
  source/FastMath.cpp
  source/FFTSpectreHandler.cpp
  source/Filter.cpp
  source/FIR.cpp
  source/FirFilter.cpp
  source/FlowingFFTSpectre.cpp
  source/FMDemodulator.cpp
  source/HilbertTransform.cpp
  source/KalmanFilter.cpp
  source/kSDR.cpp
  source/ListSetting.cpp
  source/MemoryRecordUserInterface.cpp
  source/Mixer.cpp
  source/Oscillator.cpp
  source/PolyPhaseFilter.cpp
  source/ReceiverLogic.cpp
  source/ReceiverRegionInterface.cpp
  source/RSP1.cpp
  source/Semaphore.cpp
  source/SinOscillator.cpp
  source/SMeter.cpp
  source/SoundCard.cpp
  source/SoundProcessorThread.cpp
  source/SoundReaderThread.cpp
  source/Spectre.cpp
  source/FFTData.cpp
  source/SpeedCount.cpp
  source/MyThread.cpp
  source/Utils.cpp
  source/ViewModel.cpp
  source/Waterfall.cpp
  source/WindowBlackman.cpp
  source/WindowBlackmanHarris.cpp
)

target_include_directories(ksdr PRIVATE source/include)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(ksdr fftw3 rtlsdr hackrf sdrplay_api portaudio glfw OpenGL::GL)