#pragma once

#include "Env.h"
#include <assert.h>
#include "cmath"

class FastMath {

public:
	FastMath();
	float fastcos(float n);
	float fastsin(float n);
	void FloatToInt(int* int_pointer, float f);
};