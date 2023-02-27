#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

//#define M_PI						3.1415926535897932384
//#define M_PI						3.14159
#define M_PI						3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502



#define PA_SAMPLE_TYPE				paFloat32

#define APP_NAME					"kSDR v1.0 Beta 4"

struct Signal {
	float I;
	float Q;
};

#define USB		0
#define LSB		1
#define AM		2
#define nFM		3