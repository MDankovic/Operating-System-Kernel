#include "KrnlSem.h"
#include "Semaphor.h"

Semaphore::Semaphore(int _init) {
	myImpl = new KernelSem(_init, this);
}

int Semaphore::wait(Time _maxTimeToWait) {
	return myImpl->wait(_maxTimeToWait);
}

int Semaphore::signal(int _n) {
	return myImpl->signal(_n);
}

Semaphore::~Semaphore() {
	delete myImpl;
}

int Semaphore::val() const {
	return myImpl->val();
}
