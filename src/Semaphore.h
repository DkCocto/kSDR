#pragma once
class Semaphore {
	bool barrier = false;

public:
	bool lock();
	void unlock();
	bool isLocked();
};