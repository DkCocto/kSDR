#pragma once

#include "atomic"

class MyThread {
	protected:
		std::atomic<bool> isWorking_ = false;
	public:
		bool isWorking();
};