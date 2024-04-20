#pragma once

#include "vector"
#include "Env.h"

using namespace std;

class FMDemodulator {

private:

	vector<double> i = { 0.0, 0.0, 0.0 };
	vector<double> q = { 0.0, 0.0, 0.0 };
	double lastPhase = 0.0f;
	double gain = 0.001f;
	double limiti;
	double limitq;


public:

	FMDemodulator() {
		i[0] = 0.1f;
		q.at(0) = 0.1f;
		i.at(1) = 0.1f;
		q.at(1) = 0.1f;
		i.at(2) = 0.1f;
		q.at(2) = 0.1f;
	}


	double demodulateAM(double in, double qn) {
		double mag = sqrt(in * in + qn * qn);
		return mag;
	}

	/**
	 * Arctan method. We can calculate the instantaneous angle from I and Q
	 * @param in
	 * @param qn
	 * @return
	 */
	double atanDemodulate(double in, double qn) {
		double I, Q;
		double angle = 0;

		/**
		 * Multiply the current sample against the complex conjugate of the
		 * previous sample to derive the phase delta between the two samples
		 *
		 * Negating the previous sample quadrature produces the conjugate
		 */
		I = in * i[0] - qn * -q[0];
		Q = qn * i[0] + in * -q[0];
		i[0] = in;
		q[0] = qn;

		if (I == 0.0) {
			I = 1E-20f; // small value to prevent divide by zero error
			//angle = 0;
		}
		/**
		 * Use the arc-tangent of imaginary (q) divided by real (i) to
		 * get the phase angle (+/-) which was directly manipulated by the
		 * original message waveform during the modulation.  This value now
		 * serves as the instantaneous amplitude of the demodulated signal
		 */
		double denominator = 1.0f / I;
		angle = atan(Q * denominator);

		// If both real and imaginary parts are negative, need to subtract PI radians
		if (I < 0.0 && Q < 0.0) {
			angle = (angle - M_PI);
		}
		if (I < 0.0 && Q >= 0.0) {
			angle = (angle + M_PI);
		}

		return angle * gain;
	}

	/**
	 * We pass in the current values for i and q
	 *
	 *  * To measure the change in frequency, we want d-theta/dt and we don’t want to use arctan because its hard to do at speed.
	 *
	 * r(t) = q(t)/i(t)
	 *
	 * Derivative of arctan is the identity 1/(1+r^2)
	 * Therefore d-theta/dt = 1/(1+r^2)( d[r(t)/d(t) )   (1)
	 * Also, d[r(t)]/dt = d[q(t)/i(t)/d(t) and there is an identity for the derivative of a ratio, so
	 * d[r(t)/d(t) = ( i(t) * d[q(t)]/d(t) - q(t) * d[i(t)]/d(t) ) /i^2(t)
	 *
	 * So, using this result in equation (1) above gives
	 * d-theta/d(t) = (1/(1 + r^2)(  ( i(t) * d[q(t)]/d(t) - q(t) * d[i(t)]/d(t) ) /i^2(t) )
	 *
	 * Replacing r(t) with q(t)/i(t) gives:
	 * d-theta/d(t) = (1/(1 + (q(t)^2/i(t)^2)(  ( i(t) * d[q(t)]/d(t) - q(t) * d[i(t)]/d(t) ) /i^2(t) )
	 *
	 * Multiply by i^2(t)
	 * d-theta/d(t) = i(t)d[q(t)]/d(t) - q(t)d[i(t)]/d(t)
	 *                   ------------------------------------
	 *                           i^2(t) + q^2(t)
	 *
	 * Using the central difference method for differentiation gives:
	 * d-theta(n) = i(n-1) * (q(n) - q(n-2)) - q(n-1) * (i(n) - i(n-2))
	 *                 -----------------------------------------------------------
	 *                              2* (i(n-1)^2 + q(n-1)^2)
	 *
	 * If this is pure fm signal and is limited, then the denominator is probably a constant and does not need to be calculated for each sample.
	 *
	 * i(n) is i[2]
	 * i(n-1) is i[1]
	 * i(n-2) is i[0]
	 * @param i
	 * @param q
	 * @return
	 */
	double demodulate(double fftData, double fftData2) {
		/*static complex inD;
		double A, B, S;
		A = in.Re; B = in.Im;
		S = (A * (inD.Im - B) - (inD.Re - A) * B) / (A * A + B * B);
		inD.Re = in.Re;
		inD.Im = in.Im;*/


		i[0] = i[1];
		q[0] = q[1];
		i[1] = i[2];
		q[1] = q[2];


		i[2] = fftData;
		q[2] = fftData2;

		// it simplifies to: Demodn={Qn*In-1  -  In*Qn-1}/{In2+Qn2}

		double gain = 0.001f; // magic number of 1/2 seems to work best
		double num = i[1] * (q[2] - q[0]) - q[1] * (i[2] - i[0]);
		//double num = q[2] * i[1]  - i[2] * q[1];
		double den = (i[1] * i[1] + q[1] * q[1]);

		double deltafreq = gain * (num / den);

		if (isnan(deltafreq)) {
			deltafreq = 0; // make sure we don't get locked in a bad position
		}
		return deltafreq;
	}
};