#include "MyThread.h"

void MyThread::pause() {
	currentStatus = PAUSE;
}

void MyThread::continueRead() {
	currentStatus = START_READING;
}

bool MyThread::isWorking() {
    return isWorking_;
}