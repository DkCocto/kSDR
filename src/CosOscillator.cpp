#include "CosOscillator.h"

long kaka = 0;

double CosOscillator::nextSample() {
	double value = fm.fastcos(phase);
	if (kaka % 800000 == 0) {
		printf("cos(%f) = %f; fastcos(%f) = %f\n", phase, cos(phase), phase, value);
		kaka = 0;
	}
	phase += phaseIncrement;
	kaka++;
	return value;
}