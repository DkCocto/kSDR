#include "ComplexSignal.h"

float ComplexSignal::magnitude() {
	return sqrt(I * I + Q * Q);
}

void ComplexSignal::multiply(float val) {
	I = I * val;
	Q = Q * val;
}

ComplexSignal::ComplexSignal() { }

ComplexSignal::ComplexSignal(float I, float Q) {
	this->I = I;
	this->Q = Q;
}

void ComplexSignal::normalize() {
	float m = magnitude();
	if (m != 0.0f) {
		multiply(1.0f / m);
	}
}