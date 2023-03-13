#pragma once

#include "vector"
#include "string"
#include "locale"
#include <sstream>

class Utils {

	class MyNumPunct : public std::numpunct<char> {
	protected:
		virtual char do_thousands_sep() const { return '.'; }
		virtual std::string do_grouping() const { return "\03"; }
	};

public:
	static void printArray(float* array, int len);
	static void printArray(double* array, int len);
	static void printArrayWithoutIndexes(double* array, int len);
	static void printArray(std::vector<float> v);
	static void printFloat(float v);
	static bool parse_u32(char* s, uint32_t* const value);
	static std::string getPrittyFreq(int freq);
	static std::string getShortPrittyFreq(int freq);
};
