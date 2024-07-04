#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

#define M_PI						3.141592653589f

#define PA_SAMPLE_TYPE				paFloat32

#define APP_NAME					"kSDR v1.0 Beta 11"

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
constexpr auto DISABLE_CONTROL_STORE_FREQ = 4;
constexpr auto DISABLE_CONTROL_EDIT_FREQ = 5;
constexpr auto DISABLE_CONTROL_DEL_FREQ = 6;
constexpr auto DISABLE_CONTROL_COLOR_PICKER = 7;

enum DeviceType {
	RSP,
	HACKRF,
	RTL
};

#define LIGHTRED					IM_COL32(255, 0, 0, 255)
#define GREEN						IM_COL32(0, 204, 0, 255)
#define BLACK						IM_COL32(0, 0, 0, 230)