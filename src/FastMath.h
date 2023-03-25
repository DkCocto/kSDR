#pragma once

#include "Env.h"
#include <assert.h>
#include "cmath"

constexpr auto TABLE_SIZE		= 64 * 1024;
constexpr auto TWO_PI			= 2.0f * M_PI;
constexpr auto PI_DIV_2			= M_PI / 2.0f;

inline float table[TABLE_SIZE]; // Declare table of fast cosinus and sinus

class FastMath {

public:
	FastMath();
	float fastcos(float n);
	float fastsin(float n);
	void FloatToInt(int* int_pointer, float f);


	void initTable();
	float myFastSin(float val);
	float myFastCos(float val);
};