#pragma once

#include "vector"

class FirFilter {
    std::vector<float> coeffs; // ������������ �������
    std::vector<float> xv; // ���� ������ ������ ���������� ��������
    int M; // ���������� �������� (����� �������)
    int currentIndex = 0; // ������� ������ ��� ���������� ������
    float Fc = 0.0f; // ����� ����������� ��� cutoffFreq/SAMPLE_RATE

public:
    FirFilter(std::vector<float> taps, int tabsLen) {
        coeffs = taps;
        M = tabsLen - 1;
        xv = std::vector<float>(M + 1, 0.0f); // ������������� ������ � ������
    }

    /**
     * ������������ ��������� � ������� ������� (����������).
     * ��������������, ��� ������������ ������� ��� �����������, ��� ���
     * ����� �������� ����������� � ����� ������������� ����� xv.
     * �������� ��������, ��� ������������ �������, ����� ��� ������� � �������� ���������,
     * �� ������� ����������.
     * @param in ������� ��������
     * @return ��������� ����������
     */
    float filter(float in) {
        // ��������� ����� �������� �� ����� �� ������� �������
        xv[currentIndex] = in;

        // ��������� ���������� ����� �������
        float sum = 0.0f;
        int idx = currentIndex;
        for (int i = 0; i <= M; i++) {
            sum += coeffs[i] * xv[idx];
            idx = (idx + 1) % (M + 1); // ���������� � �������������� ���������� ������
        }

        // ��������� currentIndex ��� ���������� ������
        currentIndex = (currentIndex + 1) % (M + 1);

        return sum;
    }
};

/*#pragma once

#include "vector"

class FirFilter {

	std::vector<float> coeffs;
	std::vector<float> xv;				  // This array holds the delayed values
	int M; // The number of taps, the length of the filter
	float Fc = 0.0f; // Will be set to cutoffFreq/SAMPLE_RATE; 

public:

	FirFilter(std::vector<float> taps, int tabsLen) {
		coeffs = taps;
		M = tabsLen - 1;
		xv = std::vector<float>(M + 1, 0);
		xv.reserve(M + 1);
	}

	/**
	 * Calculate the result with convolution.  This assumes that the filter kernel coeffs are already reversed as
	 * new values are added to the end of the delay line xv.  Note that a symmetrical filter such as a raised
	 * cosine does not need to be reversed.
	 * @param in
	 * @return
	 
	float filter(float in) {
		float sum = 0.0f;
		int i;
		for (i = 0; i < M; i++) xv[i] = xv[i + 1];
		xv[M] = in;
		for (i = 0; i <= M; i++) {
			sum += (coeffs[i] * xv[i]);
		}
		return sum;
	}
};*/