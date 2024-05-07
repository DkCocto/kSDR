#include "HilbertTransformFFTW.h"

HilbertTransformFFTW::HilbertTransformFFTW(int len) {
    this->len = len;

    out = new fftw_complex[len];

    planForward = fftw_plan_dft_1d(this->len, out, out, FFTW_FORWARD, FFTW_MEASURE);

    planBackward = fftw_plan_dft_1d(this->len, out, out, FFTW_BACKWARD, FFTW_MEASURE);
}

HilbertTransformFFTW::~HilbertTransformFFTW() {
    fftw_destroy_plan(planForward);
    fftw_destroy_plan(planBackward);
    fftw_cleanup();

    delete[] out;
}

void HilbertTransformFFTW::resetOut() {
    if (out != nullptr) delete[] out;
    out = new fftw_complex[len];
}

fftw_complex* HilbertTransformFFTW::process(double* in) {

    // copy the data to the complex array
    for (int i = 0; i < len; ++i) {
        out[i][REAL] = in[i];
        out[i][IMAG] = 0;
    }


    fftw_execute(planForward);

    int hN = len >> 1; // half of the length (N/2)
    int numRem = hN; // the number of remaining elements
    // multiply the appropriate value by 2
    //(those should multiplied by 1 are left intact because they wouldn't change)
    for (int i = 1; i < hN; ++i) {
        out[i][REAL] *= 2;
        out[i][IMAG] *= 2;
    }
    // if the length is even, the number of the remaining elements decrease by 1
    if (len % 2 == 0)
        numRem--;
    else if (len > 1) {
        out[hN][REAL] *= 2;
        out[hN][IMAG] *= 2;
    }
    // set the remaining value to 0
    // (multiplying by 0 gives 0, so we don't care about the multiplicands)
    memset(&out[hN + 1][REAL], 0, numRem * sizeof(fftw_complex));
    // creat a IDFT plan and execute it
    fftw_execute(planBackward);

    // scale the IDFT output
    for (int i = 0; i < len; ++i) {
        out[i][REAL] /= len;
        out[i][IMAG] /= len;
    }

    return out;
}
