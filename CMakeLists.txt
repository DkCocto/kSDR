cmake_minimum_required(VERSION 3.18)

project(ksdr
  LANGUAGES CXX C
  DESCRIPTION "Simple Fast SDR receiver app with OpenGL "
  VERSION 1.0.9
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI_EXAMPLES OFF)
set(IMGUI_DEMO OFF)
set(IMGUI_DISABLE_OBSOLETE_FUNCTIONS OFF)
set(IMGUI_DISABLE_OBSOLETE_KEYIO OFF)
set(IMGUI_ENABLE_FREETYPE OFF)
set(IMGUI_ENABLE_STDLIB_SUPPORT ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wunused-parameter -Wformat -Wnarrowing -Wno-sign-conversion -Wno-error=declaration-after-statement)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wnontrivial-memaccess)
    endif ()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
endif()

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

add_subdirectory(libraries)

target_include_directories(ksdr PRIVATE source/include)

find_package(OpenGL)

if (WIN32)
  target_link_libraries(ksdr PRIVATE 
    libusb-1.0 # HackRF dependency
    pthreadVC2 # HackRF dependency
    fftw3 
    hackrf 
    rtlsdr 
    sdrplay 
    stb 
    PortAudio 
    glfw 
    OpenGL::GL 
    tinyxml2 
    imgui 
    imgui-stdlib 
    imgui-opengl3 
    imgui-glfw
  )
  target_include_directories(ksdr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libraries/portaudio/include")
else()
  target_link_libraries(ksdr PRIVATE 
    fftw3 
    hackrf 
    rtlsdr 
    sdrplay_api 
    PortAudio 
    glfw 
    OpenGL::GL 
    tinyxml2 
    imgui 
    imgui-stdlib 
    imgui-opengl3 
    imgui-glfw
  )
endif()