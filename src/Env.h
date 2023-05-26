#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

#define M_PI						3.141592653589f

#define PA_SAMPLE_TYPE				paFloat32

#define APP_NAME					"kSDR v1.0 Beta 7"

struct Signal {
	float I;
	float Q;
};

#define USB		0
#define LSB		1
#define AM		2
#define nFM		3
#define DSB		4

constexpr auto DISABLE_CONTROL_SPECTRE_BUTTONS = 0;
constexpr auto DISABLE_CONTROL_DIALOG = 1;
constexpr auto DISABLE_CONTROL_WINDOW_ON_TOP = 2;
constexpr auto DISABLE_ON_MEMORY_MARKER = 3;

enum DeviceType {
	RSP,
	HACKRF,
	RTL
};