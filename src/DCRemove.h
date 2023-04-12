#pragma once

class DCRemove {
	float p1 = 0;
	float p2 = 0;
	float xm1 = 0;
	float xm2 = 0;
	float ym1 = 0;
	float ym2 = 0;

	const float FACTOR = 0.9999f;

public:

	void process(float *signalI, float *signalQ);
};