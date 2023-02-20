#include "DCRemove.h"

void DCRemove::process(float *signalI, float *signalQ) {
	/*p1 = data[2 * i];
	data[2 * i] = p1 - xm1 + 0.9995 * ym1;
	xm1 = p1;
	ym1 = data[2 * i];*/

	p1 = *signalI;
	*signalI = p1 - xm1 + FACTOR * ym1;
	xm1 = p1;
	ym1 = *signalI;

	p2 = *signalQ;
	*signalQ = p2 - xm2 + FACTOR * ym2;
	xm2 = p2;
	ym2 = *signalQ;
}