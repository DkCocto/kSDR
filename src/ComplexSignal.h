#pragma once
#include "cmath"

class ComplexSignal {

	double magnitude();

	void multiply(double val);

public: 

	double I = 0.0f;
	double Q = 0.0f;

	ComplexSignal();

	ComplexSignal(double I, double Q);

	void normalize();
};