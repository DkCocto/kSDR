cmake_minimum_required(VERSION 3.18)
project(ksdr)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI_EXAMPLES OFF)
set(IMGUI_DEMO OFF)

set(IMGUI_DISABLE_OBSOLETE_KEYIO OFF)
set(IMGUI_ENABLE_FREETYPE OFF)
set(IMGUI_DISABLE_OBSOLETE_KEYIO OFF)

set(IMGUI_ENABLE_STDLIB_SUPPORT ON)

find_package(OpenGL)

add_subdirectory(libraries)

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

IF (WIN32)
  target_link_libraries(ksdr PRIVATE fftw hackrf rtlsdr sdrplay stb PortAudio glfw OpenGL::GL tinyxml2 imgui imgui-stdlib imgui-opengl3 imgui-glfw)
  target_include_directories(ksdr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libraries/portaudio/include")

ELSE()
  target_link_libraries(ksdr PRIVATE fftw3 hackrf rtlsdr sdrplay_api PortAudio glfw OpenGL::GL tinyxml2 imgui imgui-stdlib imgui-opengl3 imgui-glfw)
ENDIF()