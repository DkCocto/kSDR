#pragma once

#include "atomic"

class Semaphore {
	std::atomic_bool barrier = false;

public:
	bool lock();
	void unlock();
	bool isLocked();
};