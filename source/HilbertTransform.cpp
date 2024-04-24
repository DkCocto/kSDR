#include "HilbertTransform.h"
#include "FastMath.h"

HilbertTransform::HilbertTransform(int samplingRate, int len) {
    this->samplingRate = samplingRate;
    this->len = len;
    init();
    M = len - 1;
    xv = new float[len];
    memset(xv, 0, sizeof(float) * len);
}

HilbertTransform::~HilbertTransform() {
    delete[] xv;
    delete[] coeffs;
}

void HilbertTransform::init() {
    float* tempCoeffs = new float[len];
    float sumofsquares = 0.0f;

    for (int n = 0; n < len; n++) {
        if (n == len / 2) {
            tempCoeffs[n] = 0.0f;
        }
        else {
            tempCoeffs[n] = samplingRate / (M_PI * (n - (len - 1) / 2.0f)) * (1.0f - cos(M_PI * (n - (len - 1) / 2.0f)));

        }
        sumofsquares += tempCoeffs[n] * tempCoeffs[n];
    }
    gain = sqrt(sumofsquares); ///tempCoeffs.length;

    coeffs = new float[len];
    for (int i = 0; i < len; i++) {
        coeffs[i] = tempCoeffs[len - i - 1] / gain;
    }

    delete[] tempCoeffs;
}

float HilbertTransform::filter(float val) {
    float sum;
    int i;
    for (i = 0; i < M; i++) xv[i] = xv[i + 1];
    xv[M] = val;
    sum = 0.0f;
    for (i = 0; i <= M; i++) sum += coeffs[i] * xv[i];
    return sum;
}