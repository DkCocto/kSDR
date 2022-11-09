#include "Filter.h"

double* Filter::makeRaiseCosine(double sampleRate, double freq, double alpha, int len) {
    int M = len - 1;
    double* coeffs = new double[len];
    double Fc = freq / sampleRate;

    double sumofsquares = 0;
    double* tempCoeffs = new double[len];

    int limit = (int)(0.5 / (alpha * Fc));

    for (int i = 0; i <= M; i++) {
        double sinc = (sin(2 * M_PI * Fc * (i - M / 2.0))) / (i - M / 2.0);
        double cos_ = cos(alpha * M_PI * Fc * (i - M / 2.0)) / (1 - (pow((2 * alpha * Fc * (i - M / 2.0)), 2)));

        if (i == M / 2) {
            tempCoeffs[i] = 2 * M_PI * Fc * cos_;
        } else {
            tempCoeffs[i] = sinc * cos_;
        }

        // Care because ( 1 - ( 2 * Math.pow((alpha * Fc * (i - M/2)),2))) is zero for
        if ((i - M / 2) == limit || (i - M / 2) == -limit) {
            tempCoeffs[i] = 0.25 * M_PI * sinc;
        }

        sumofsquares += tempCoeffs[i] * tempCoeffs[i];
    }
    double gain = sqrt(sumofsquares);
    for (int i = 0; i < len; i++) {
        coeffs[i] = tempCoeffs[len - i - 1] / gain;
    }
    return coeffs;
}