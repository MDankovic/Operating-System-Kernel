#include "Event.h"
#include "KrnlEv.h"

// Just wrapper methods
// Same as Thread and PCB

Event::Event(IVTNo ivtNo) {
	myImpl = new KernelEv(ivtNo, this);
}

Event::~Event() {
	delete myImpl;
}

void Event::wait() {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}



