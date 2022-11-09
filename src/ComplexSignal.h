#pragma once
#include "cmath"

class ComplexSignal {

	double magnitude();

	void multiply(double val);

public: 

	double I = 0;
	double Q = 0;

	ComplexSignal(double I, double Q);

	void normalize();

};