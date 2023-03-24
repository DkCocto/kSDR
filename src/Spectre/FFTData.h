#pragma once

#include "memory"
#include "mutex"
#include "vector"

class FFTData {

public:

	struct OUTPUT {
		float* data;
		int len;
	};

private:

	std::vector<float> spectreData;
	std::vector<float> shadowSpectreData;

	std::vector<float> waterfallData;
	std::vector<float> shadowWaterfallData;

	OUTPUT* spectreDataN;
	OUTPUT* shadowSpectreDataN;

	OUTPUT* waterfallDataN;
	OUTPUT* shadowWaterfallDataN;

	std::mutex mutex;

public:

	FFTData(int startSpectreLen);

	void setData(float* spectreData, float* waterfallData, int len);

	OUTPUT* getDataCopy(bool waterfall);
	OUTPUT* getDataCopy(int startPos, int len, bool waterfall);
	OUTPUT* getDataCopy(OUTPUT* data, int startPos, int len, bool waterfall);

	void destroyData(OUTPUT* data);

	/*void setData(float* spectreData, float* waterfallData, int len);
	std::vector<float> getData(bool waterfall);
	std::vector<float> getData(int startPos, int len, bool waterfall);*/

};