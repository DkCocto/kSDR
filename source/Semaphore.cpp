#include "Semaphore.h"

bool Semaphore::lock() {
	if (isLocked()) {
		return false;
	}
	else {
		return barrier = true;
	}
}
void Semaphore::unlock() {
	barrier = false;
}
bool Semaphore::isLocked() {
	return barrier;
}