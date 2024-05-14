#pragma once

#include "atomic"

class MyThread {
	protected:
		std::atomic<bool> isWorking_ = false;

		enum Status {
			START_READING,
			PAUSE,
			REST,
			READING
		} currentStatus = REST;

	public:
		bool isWorking();
		void pause();
		void continueRead();
};