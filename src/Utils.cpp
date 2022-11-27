#include "Utils.h"

void Utils::printArray(float* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("%i %f\r\n", i, array[i]);
	}
}

void Utils::printArray(double* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("%i %f\r\n", i, array[i]);
	}
}
void Utils::printArray(std::vector<float> v) {
	for (int i = 0; i < v.size(); i++) {
		printf("%i %f\r\n", i, v[i]);
	}
}

void Utils::printFloat(float v) {
	printf("%f\r\n", v);
}
