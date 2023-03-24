#include "FFTData.h"

#include "../Semaphore.h"

Semaphore s;

FFTData::FFTData(int startSpectreLen) {
	
	spectreDataN = new OUTPUT { new float[startSpectreLen], startSpectreLen };
	memset(spectreDataN->data, 0, sizeof(float) * spectreDataN->len);

	waterfallDataN = new OUTPUT{ new float[startSpectreLen], startSpectreLen };
	memset(waterfallDataN->data, 0, sizeof(float) * waterfallDataN->len);

	shadowSpectreDataN = new OUTPUT{ new float[startSpectreLen], startSpectreLen };
	memset(shadowSpectreDataN->data, 0, sizeof(float) * shadowSpectreDataN->len);

	shadowWaterfallDataN = new OUTPUT{ new float[startSpectreLen], startSpectreLen };
	memset(shadowWaterfallDataN->data, 0, sizeof(float) * shadowWaterfallDataN->len);

	/*spectreData = std::vector<float>(startSpectreLen);
	std::fill(spectreData.begin(), spectreData.end(), 0);

	waterfallData = std::vector<float>(startSpectreLen);
	std::fill(waterfallData.begin(), waterfallData.end(), 0);

	shadowSpectreData = std::vector<float>(startSpectreLen);
	std::fill(shadowSpectreData.begin(), shadowSpectreData.end(), 0);

	shadowWaterfallData = std::vector<float>(startSpectreLen);
	std::fill(shadowWaterfallData.begin(), shadowWaterfallData.end(), 0);*/
}

void FFTData::setData(float* spectreData, float* waterfallData, int len) {
	if (!mutex.try_lock()) return;

	if (spectreDataN->len != len) {
		delete[] spectreDataN->data;
		spectreDataN->data = new float[len];
		spectreDataN->len = len;
	}

	memcpy(spectreDataN->data, spectreData, sizeof(spectreData) * len);

	if (waterfallDataN->len != len) {
		delete[] waterfallDataN->data;
		waterfallDataN->data = new float[len];
		waterfallDataN->len = len;
	}

	memcpy(waterfallDataN->data, waterfallData, sizeof(waterfallData) * len);

	//this->spectreData.assign(spectreData, spectreData + len);
	//this->waterfallData.assign(waterfallData, waterfallData + len);

	mutex.unlock();
}

FFTData::OUTPUT* FFTData::getDataCopy(bool waterfall) {
	int size = (waterfall == true) ? waterfallDataN->len : spectreDataN->len;
	int sizeDiv2 = size / 2;
	
	if (!mutex.try_lock()) {
		OUTPUT* dataCopy = new OUTPUT{ new float[size], size };
		if (!waterfall) {
			memcpy(dataCopy->data, shadowSpectreDataN->data, sizeof(float) * size);
		} else {
			memcpy(dataCopy->data, shadowWaterfallDataN->data, sizeof(float) * size);
		}
		return dataCopy;
	}
	
	OUTPUT* dataCopy = new OUTPUT { new float[size], size };

	//memcpy(buffer, data + (spectreSize / 2), sizeof(data) * (spectreSize / 2));
	//memcpy(buffer + (spectreSize / 2), data, sizeof(data) * (spectreSize / 2));
	if (!waterfall) {
		memcpy(dataCopy->data, spectreDataN->data + sizeDiv2, sizeof(float) * sizeDiv2);
		memcpy(dataCopy->data + sizeDiv2, spectreDataN->data, sizeof(float) * sizeDiv2);

		destroyData(shadowSpectreDataN);
		shadowSpectreDataN = new OUTPUT { new float[size], size };
		memcpy(shadowSpectreDataN->data, dataCopy->data, sizeof(float) * size);
	} else {
		memcpy(dataCopy->data, waterfallDataN->data + sizeDiv2, sizeof(float) * sizeDiv2);
		memcpy(dataCopy->data + sizeDiv2, waterfallDataN->data, sizeof(float) * sizeDiv2);

		destroyData(shadowWaterfallDataN);
		shadowWaterfallDataN = new OUTPUT { new float[size], size };
		memcpy(shadowWaterfallDataN->data, dataCopy->data, sizeof(float) * size);
	}

	mutex.unlock();

	return dataCopy;
}

/// <summary>
/// ¬озвращает кусок из данных. Ќе забывать очищать данные после использовани€.
/// </summary>
/// <param name="startPos"></param>
/// <param name="len"></param>
/// <param name="waterfall"></param>
/// <returns></returns>
FFTData::OUTPUT* FFTData::getDataCopy(int startPos, int len, bool waterfall) {
	FFTData::OUTPUT* data = getDataCopy(waterfall);

	OUTPUT* dataCopy = new OUTPUT{ new float[len], len };

	memcpy(dataCopy->data, data->data + startPos, sizeof(float) * len);
	
	destroyData(data);

	return dataCopy;
}

FFTData::OUTPUT* FFTData::getDataCopy(OUTPUT* data, int startPos, int len, bool waterfall) {
	OUTPUT* dataCopy = new OUTPUT{ new float[len], len };
	memcpy(dataCopy->data, data->data + startPos, sizeof(float) * len);
	return dataCopy;
}

void FFTData::destroyData(OUTPUT* data) {
	delete[] data->data;
	delete data;
}

/*std::vector<float> FFTData::getData(int startPos, int len, bool waterfall) {
	std::vector<float> data = getData(waterfall);
	std::vector<float> dataCopy(len);

	//memcpy(dataCopy, buffer + startPos, sizeof(float) * len);
	std::copy(data.begin() + startPos, data.begin() + startPos + len, dataCopy.begin());

	return dataCopy;
}*/

/*void FFTData::setData(float* spectreData, float* waterfallData, int len) {
	if (!mutex.try_lock()) return;

	this->spectreData.assign(spectreData, spectreData + len);
	this->waterfallData.assign(waterfallData, waterfallData + len);

	mutex.unlock();
}*/

/*std::vector<float> FFTData::getData(bool waterfall) {
	if (!mutex.try_lock()) {
		if (!waterfall) {
			return shadowSpectreData;
		} else {
			return shadowWaterfallData;
		}
	}

	int size = (waterfall == true) ? waterfallData.size() : spectreData.size();
	int sizeDiv2 = size / 2;

	std::vector<float> dataCopy(size);
	//memcpy(buffer, data + (spectreSize / 2), sizeof(data) * (spectreSize / 2));
	//memcpy(buffer + (spectreSize / 2), data, sizeof(data) * (spectreSize / 2));
	if (!waterfall) {
		std::copy(spectreData.begin() + sizeDiv2, spectreData.end(), dataCopy.begin());
		std::copy(spectreData.begin(), spectreData.end() - sizeDiv2, dataCopy.begin() + sizeDiv2);

		shadowSpectreData = dataCopy;
	} else {
		std::copy(waterfallData.begin() + sizeDiv2, waterfallData.end(), dataCopy.begin());
		std::copy(waterfallData.begin(), waterfallData.end() - sizeDiv2, dataCopy.begin() + sizeDiv2);

		shadowWaterfallData = dataCopy;
	}

	mutex.unlock();

	return dataCopy;
}*/