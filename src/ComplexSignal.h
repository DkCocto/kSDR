#pragma once
#include "cmath"

class ComplexSignal {

	double magnitude();

	void multiply(double val);

public: 

	double I = 0.0;
	double Q = 0.0;

	ComplexSignal();

	ComplexSignal(double I, double Q);

	void normalize();

};