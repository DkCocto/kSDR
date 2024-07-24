#pragma once

#include "vector"
#include "string"
#include "locale"
#include <sstream>
#include "iostream"
#include <iomanip>
#include "Windows.h"

#define watch(x) cout << (#x) << " is " << (x) << endl



class Utils {

	class MyNumPunct : public std::numpunct<char> {
	protected:
		virtual char do_thousands_sep() const { return '.'; }
		virtual std::string do_grouping() const { return "\03"; }
	};

public:

	enum Modulation_ {
		USB_,
		LSB_,
		AM_,
		nFM_,
		DSB_
	} modulation_;

	static void printArray(float* array, int len);
	static void printArray(double* array, int len);
	static void printArray(int* array, int len);
	static void printArrayWithoutIndexes(double* array, int len);
	static void printArray(std::vector<float> v);
	static void printFloat(float v);
	static bool parse_u32(char* s, uint32_t* const value);
	static std::string getPrittyFreq(int freq);
	static std::string getShortPrittyFreq(int freq);
	static std::string toStrWithAccuracy(float val, int accuracy);
	static std::string getPrittyFilterWidth(int width);

	static double convertSegment(double value, double From1, double From2, double To1, double To2);

	static int convFFTResBinToSpecBin(int bin, int len);

	static std::string getModulationTxt(int modulationCode) {
		switch (modulationCode) {
		case USB_:
			return "USB";
			break;
		case LSB_:
			return "LSB";
			break;
		case AM_:
			return "AM";
			break;
		case nFM_:
			return "nFM";
			break;
		case DSB_:
			return "DSB";
			break;
		default:
			return "USB";
		}
	}

	static Modulation_ getModulation(int modulationCode) {
		return Modulation_(modulationCode);
	}
};
