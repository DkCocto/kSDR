#pragma once

#include "vector"

class Utils {
public:
	static void printArray(float* array, int len);
	static void printArray(double* array, int len);
	static void printArrayWithoutIndexes(double* array, int len);
	static void printArray(std::vector<float> v);
	static void printFloat(float v);
	static bool parse_u32(char* s, uint32_t* const value);
};
