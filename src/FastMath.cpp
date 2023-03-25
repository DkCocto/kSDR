#include "FastMath.h"

FastMath::FastMath() {
	// Build cossin table
	for (long i = 0; i < MAX_CIRCLE_ANGLE; i++) {
		fast_cossin_table[i] = (float)sin((double)i * M_PI / HALF_MAX_CIRCLE_ANGLE);
	}
	initTable();
}

float FastMath::fastcos(float n) {
	double f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
	int i = (int)f;
	//FloatToInt(&i, f);
	if (i < 0) {
		return fast_cossin_table[((-i) + QUARTER_MAX_CIRCLE_ANGLE) & MASK_MAX_CIRCLE_ANGLE];
	} else {
		return fast_cossin_table[(i + QUARTER_MAX_CIRCLE_ANGLE) & MASK_MAX_CIRCLE_ANGLE];
	}

	//assert(0);
}

float FastMath::fastsin(float n) {
	double f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
	int i = (int)f;
	//FloatToInt(&i, f);
	if (i < 0) {
		return fast_cossin_table[(-((-i) & MASK_MAX_CIRCLE_ANGLE)) + MAX_CIRCLE_ANGLE];
	} else {
		return fast_cossin_table[i & MASK_MAX_CIRCLE_ANGLE];
	}

	//assert(0);
}

void FastMath::FloatToInt(int* int_pointer, float f) {
	__asm  fld  f
	__asm  mov  edx, int_pointer
	__asm  FRNDINT
	__asm  fistp dword ptr[edx];
}

void FastMath::initTable() {
	for (int i = 0; i < TABLE_SIZE; i++) {
		table[i] = (float)sin(TWO_PI * (double)i / TABLE_SIZE);
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