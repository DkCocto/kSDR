#pragma once

#include "stdio.h"
#include "thread"
#include "Utils.h"

#define M_PI						3.1415926535897932384

#define PA_SAMPLE_TYPE				paFloat32

#define APP_NAME					"kSDR v1.0 Beta 3"

struct Signal {
	float I;
	float Q;
};

#define USB		0
#define LSB		1
#define AM		2
#define nFM		3