#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

#define M_PI						3.141592653589f
//#define M_PI						3.14159
//#define M_PI						3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502


#define MAX_CIRCLE_ANGLE      8 * 1024
#define HALF_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/2)
#define QUARTER_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/4)
#define MASK_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE - 1)
inline float fast_cossin_table[MAX_CIRCLE_ANGLE];           // Declare table of fast cosinus and sinus

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

constexpr auto DISABLE_CONTROL_SPECTRE_BUTTONS = 0;
constexpr auto DISABLE_CONTROL_DIALOG = 1;
constexpr auto DISABLE_CONTROL_WINDOW_ON_TOP = 2;
constexpr auto DISABLE_ON_MEMORY_MARKER = 3;

double inline __declspec (naked) __fastcall fastSqrt(double n) {
	_asm fld qword ptr[esp + 4]
	_asm fsqrt
	_asm ret 8
}

enum DeviceType {
	RSP,
	HACKRF,
	RTL
};