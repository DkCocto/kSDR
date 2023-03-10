#include "ComplexSignal.h"

double ComplexSignal::magnitude() {
	return sqrt(I * I + Q * Q);
}

void ComplexSignal::multiply(double val) {
	I = I * val;
	Q = Q * val;
}

ComplexSignal::ComplexSignal() { }

ComplexSignal::ComplexSignal(double I, double Q) {
	this->I = I;
	this->Q = Q;
}

void ComplexSignal::normalize() {
	float m = magnitude();
	if (m != 0.0) {
		multiply(1.0 / m);
	}
}