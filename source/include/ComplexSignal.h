#pragma once

#include "Env.h"
#include "cmath"
#include "Utils.h"

class ComplexSignal {

	float magnitude();

	void multiply(float val);

public: 

	float I = 0.0f;
	float Q = 0.0f;

	ComplexSignal();

	ComplexSignal(float I, float Q);

	void normalize();
};