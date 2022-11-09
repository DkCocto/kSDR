#include "HilbertTransform.h"

HilbertTransform::HilbertTransform(int samplingRate, int len) {
    this->samplingRate = samplingRate;
    this->len = len;
    init();
    M = len - 1;
    xv = new double[len];
    memset(xv, 0, sizeof(double) * len);
}

void HilbertTransform::init() {
    double* tempCoeffs = new double[len];
    double sumofsquares = 0.0;

    for (int n = 0; n < len; n++) {
        if (n == len / 2) {
            //printf("siska %i\r\n", n);
            tempCoeffs[n] = 0;
        }
        else {
            tempCoeffs[n] = samplingRate / (M_PI * (n - (len - 1) / 2.0)) * (1.0 - cos(M_PI * (n - (len - 1) / 2.0)));

        }
        sumofsquares += tempCoeffs[n] * tempCoeffs[n];
    }

    //Utils::printArray(tempCoeffs, len);

    gain = sqrt(sumofsquares); ///tempCoeffs.length;
    /*cout << "sumofsquares: " << sumofsquares << "\r\n";
    cout << "Hilbert Transform GAIN: " << gain << "\r\n";*/

    //exit(0);
    // flip
    coeffs = new double[len];
    for (int i = 0; i < len; i++) {
        coeffs[i] = tempCoeffs[len - i - 1] / gain;
    }
}

double HilbertTransform::filter(double val) {
    double sum;
    int i;
    for (i = 0; i < M; i++) xv[i] = xv[i + 1];
    xv[M] = val;
    sum = 0.0;
    for (i = 0; i <= M; i++) sum += coeffs[i] * xv[i];
    return sum;
}