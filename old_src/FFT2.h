#pragma once

#include <iostream>
#include <complex>
#define MAX 200

using namespace std;

#define M_PI 3.1415926535897932384

class FFT2 {

    int log2(int N) {
        int k = N, i = 0;
        while (k) {
            k >>= 1;
            i++;
        }
        return i - 1;
    }

    int check(int n) {
        return n > 0 && (n & (n - 1)) == 0;
    }

    //calculating revers number
    int reverse(int N, int n) {
        int j, p = 0;
        for (j = 1; j <= log2(N); j++) {
            if (n & (1 << (log2(N) - j)))
                p |= 1 << (j - 1);
        }
        return p;
    }

    //using the reverse order in the array
    void ordina(complex<float>* f1, int N) {
        complex<float> f2[MAX];
        for (int i = 0; i < N; i++)
            f2[i] = f1[reverse(N, i)];
        for (int j = 0; j < N; j++)
            f1[j] = f2[j];
    }

    void transform(complex<float>* f, int N) {
        ordina(f, N);    //first: reverse order
        complex<float>* W;
        W = (complex<float> *)malloc(N / 2 * sizeof(complex<float>));
        W[1] = polar(1., -2. * M_PI / N);
        W[0] = 1;
        for (int i = 2; i < N / 2; i++)
            W[i] = pow(W[1], i);
        int n = 1;
        int a = N / 2;
        for (int j = 0; j < log2(N); j++) {
            for (int i = 0; i < N; i++) {
                if (!(i & n)) {
                    complex<float> temp = f[i];
                    complex<float> Temp = W[(i * a) % (n * a)] * f[i + n];
                    f[i] = temp + Temp;
                    f[i + n] = temp - Temp;
                }
            }
            n *= 2;
            a = a / 2;
        }
        free(W);
    }

public:
    void FFT(complex<float>* f, int N, float d) {
        transform(f, N);
        for (int i = 0; i < N; i++) f[i] *= d; //multiplying by step
    }

};

