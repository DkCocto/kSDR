#include "FastMath.h"

FastMath::FastMath() {
	initTable();
}

void FastMath::initTable() {
	for (int i = 0; i < TABLE_SIZE; i++) {
		table[i] = (float)sin(TWO_PI * i / TABLE_SIZE);
	}
}

float FastMath::myFastSin(float val) {
	val -= (int)(val / TWO_PI) * TWO_PI;
	return ((val >= 0) ? (1.0f) : (-1.0f)) * table[(int)(abs(val) * (TABLE_SIZE / TWO_PI))];
}

float FastMath::myFastCos(float val) {
	val -= PI_DIV_2;
	return -myFastSin(val);
}