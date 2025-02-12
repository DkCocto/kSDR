#pragma once

#include "vector"
#include "FirFilter.h"
#include "Filter.h"
#include "FIR.h"

class PolyPhaseFilter {
	std::vector<float> coeffs;
	float alpha = 0.5f;
	std::vector<std::unique_ptr<FirFilter>> subFilters; // ������������� ����� ����������

private:
	void initSubFilters(const std::vector<float>& coeffs, int coeffsLen, int R) {
		int M = coeffsLen;
		subFilters.reserve(R); // ��������������� ��������� ������ ��� �����������

		int P = R - 1; // ������� ������������� ����������� �������

		for (int j = 0; j < R; j++) {
			int tabsLen = M / R;
			std::vector<float> taps(tabsLen);

			// ���������� ������������� ��� ������� ����������
			for (int i = 0; i < tabsLen; i++) {
				taps[i] = coeffs[P + i * R];
			}

			subFilters.push_back(std::make_unique<FirFilter>(taps, tabsLen));
			P = (P - 1 + R) % R; // ���������� ������� ������������� � ����������� ��������
		}
	}

public:
	PolyPhaseFilter() { }

	/**
	 * ������������� ������������� �������
	 * @param sampleRate ������� �������������
	 * @param freq ������� ����������
	 * @param decimationRate ����������� ������������
	 * @param len ����� �������������
	 */
	void initCoeffs(float sampleRate, float freq, int decimationRate, int len) {
		coeffs.clear();
		FIR fir;
		fir.init(fir.LOWPASS, fir.HAMMING, len, freq, 0, sampleRate);
		coeffs = fir.getCoeffs();

		printf("PolyPhase Filter. Len: %d\r\n", coeffs.size());
		initSubFilters(coeffs, len, decimationRate);
	}

	/**
	 * ��������� ������ �� ������� �������
	 * @param in ������� ������
	 * @param inLen ����� �������� �������
	 * @return ��������� ����������
	 */
	float filter(float* in, int inLen) {
		float sum = 0.0;
		int j = inLen - 1;

		// ���������� ������� ����������
		for (int i = 0; i < inLen; i++) {
			sum += subFilters[i]->filter(in[j--]); // ������������� ������ ���������
		}

		return sum;
	}
};

/*#pragma once

#include "vector"
#include "FirFilter.h"
#include "Filter.h"
#include "FIR.h"

class PolyPhaseFilter {
	std::vector<float> coeffs;
	float alpha = 0.5f;
	std::vector<FirFilter> subFilters;

private:

	void initSubFilters(std::vector<float> coeffs, int coeffsLen, int R) {
		int M = coeffsLen;

		subFilters.clear();

		//subFilters = new std::vector<FirFilter>[R];
		int P = R - 1; // position of the polyphase switch

		for (int j = 0; j < R; j++) {
			int tabsLen = M / R;
			std::vector<float> taps(tabsLen);
			for (int i = 0; i < tabsLen; i++) {
				taps[i] = coeffs[P + i * R];
			}
			subFilters.push_back(FirFilter(taps, tabsLen));
			P = P - 1;
			if (P < 0) P = R - 1;
		}
	}

public:

	/**
	* Taps LEN must be an integer number of decimationRate
	* @param sampleRate
	* @param freq
	* @param decimationRate
	* @param len
	
	void initCoeffs(float sampleRate, float freq, int decimationRate, int len) {
		coeffs.clear();
		FIR fir;
		fir.init(fir.LOWPASS, fir.HAMMING, len, freq, 0, sampleRate);

		coeffs = fir.getCoeffs();

		//coeffs = Filter::makeRaiseCosine(sampleRate, freq, alpha, len);

		printf("PolyPhase Filter. Len: %d\r\n", coeffs.size());
		initSubFilters(coeffs, len, decimationRate);
	}

	PolyPhaseFilter() { }

	/**
		* Each time we get an input we rotate though the coefficients based on the decimate rate R and the position P.
		* We pass in enough coefficients to perform a complete rotation, because that produces one output value. It
		* is up to the calling routine to store the values and pass them in as a set.
		* @param in
		* @return
		
	float filter(float* in, int inLen) {
		float sum = 0.0;
		int j = inLen - 1;
		for (int i = 0; i < inLen; i++) {
			sum += subFilters.at(i).filter(in[j--]);
		}
		return sum;
	}
};*/