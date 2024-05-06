#pragma once

class Modulation {

private:


protected:
	int freq = 0;

public:
	virtual void setFreq(int freq) = 0;
	int getFreq();
};