#include "Utils.h"
#include "Spectre.h"

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

void Utils::printArray(int* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("[%i]=%i\r\n", i, array[i]);
	}
}

void Utils::printArrayWithoutIndexes(double* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("%10.10f\r\n", array[i]);
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

bool Utils::parse_u32(char* s, uint32_t* const value) {
	uint_fast8_t base = 10;
	char* s_end;
	uint64_t ulong_value;

	if (strlen(s) > 2) {
		if (s[0] == '0') {
			if ((s[1] == 'x') || (s[1] == 'X')) {
				base = 16;
				s += 2;
			}
			else if ((s[1] == 'b') || (s[1] == 'B')) {
				base = 2;
				s += 2;
			}
		}
	}

	s_end = s;
	ulong_value = strtoul(s, &s_end, base);
	if ((s != s_end) && (*s_end == 0)) {
		*value = (uint32_t)ulong_value;
		return true;
	}
	else {
		return false;
	}
}

std::string Utils::getPrittyFreq(int freq) {
	std::ostringstream ss;
	ss.imbue(std::locale(std::locale::classic(), new MyNumPunct));
	ss << freq;
	return ss.str();
}

std::string Utils::getShortPrittyFreq(int freq) {
	int freqDigitsCount = std::to_string(freq).size();

	if (freqDigitsCount > 6) {
		return Utils::getPrittyFreq(freq / 1000);
	} else {
		return Utils::getPrittyFreq(freq);
	}
}

std::string Utils::toStrWithAccuracy(float val, int accuracy) {
	std::stringstream out;
	out << std::fixed << std::setprecision(accuracy) << val;
	return out.str();
}

std::string Utils::getPrittyFilterWidth(int width) {
	if (width < 1000) return std::to_string(width);
	else {
		std::stringstream out;
		out << std::fixed << std::setprecision(1) << (float)width / 1000.0f;
		return out.str();
	}
}

float Utils::getExponentialValue(int startX1, int startY1, int startX2, int startY2, int val) {

	return 0.0f;
}

double Utils::convertSegment(double value, double From1, double From2, double To1, double To2) {
	return (value - From1) / (From2 - From1) * (To2 - To1) + To1;
}

int Utils::convFFTResBinToSpecBin(int bin, int len) {
	int halfLen = len / 2;
	if (bin >= halfLen) return bin - halfLen;
	else return bin + halfLen;
}