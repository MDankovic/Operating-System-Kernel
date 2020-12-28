#include "PCB.h"
#include "System.h"
#include "Thread.h"

// Thread constructor
Thread::Thread(StackSize _stackSize, Time _timeSlice) {
	myPCB = new PCB(this, _stackSize, _timeSlice);
}	

// System call (context switch)
void dispatch() {
	System::dispatch();
}

void Thread::start() {
	myPCB->start();
}

// waitToComplete should be called from destructor in derived class
Thread::~Thread() {
	waitToComplete();
	delete myPCB;
}

void Thread::waitToComplete() {
	myPCB->waitToComplete(System::running);
}

int Thread::getId() {
	return myPCB->getID();
}

ID Thread::getRunningId() {
	return System::running->getID();
}

Thread *Thread::getThreadById(ID id) {
	return VectorPCB::getPCB(id)->myThread;
}

// Signal wrapper methods
#ifdef SIGNAL
void Thread::signal(SignalId signal) {
	myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	myPCB->registerHandler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalId id) {
	myPCB->unregisterAllHandlers(id);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	myPCB->swap(id, hand1, hand2);
}

void Thread::blockSignal(SignalId signal) {
	myPCB->blockSignal(signal);
}

void Thread::blockSignalGlobally(SignalId signal) {
	PCB::blockSignalGlobally(signal);
}

void Thread::unblockSignal(SignalId signal) {
	myPCB->unblockSignal(signal);
}

void Thread::unblockSignalGlobally(SignalId signal) {
	PCB::unblockSignalGlobally(signal);
}
#endif


// TODO UNUSED should be deleted
//void Thread::sleep(Time _timeToSleep) {
//	System::sleep(System::running, _timeToSleep);
//}
