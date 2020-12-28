#include "KrnlSem.h"
#include "Queue.h"
#include "TQueue.h"
#include "Thread.h"
#include "PCB.h"
#include "System.h"
#include "SCHEDULE.H"

#include <stdio.h>
#include "Semaphor.h"

KernelSem::KernelSem(int _init, Semaphore *_mySemaphore) : value(_init), mySemaphore(_mySemaphore) {
#ifndef BCC_BLOCK_IGNORE
	lock

	waitList = new Queue();
	timedWaitList = new TimerQueue();
	id = VectorSemaphore::putSemaphore(this);

	unlock
#endif
}

// TODO Mozda moram prvo da oslobodim sve niti koje su blokirane na ovom semaforu
KernelSem::~KernelSem() {
#ifndef BCC_BLOCK_IGNORE
	lock

	delete waitList;
	delete timedWaitList;
	VectorSemaphore::removeSemaphore(id);

	unlock
#endif
}

int KernelSem::val() const {
	return value;
}

// TODO
int KernelSem::wait(Time _maxTimeToWait) {
#ifndef BCC_BLOCK_IGNORE
	lock

	// Pas this semaphore
	if(value-- > 0) {
		unlock
		return 1;
	}

	// Block on this semaphore
	System::running->state = PCB::BLOCKED;
	if(_maxTimeToWait == 0) {
		waitList->put(System::running);
	}
	else {
		timedWaitList->put(System::running, _maxTimeToWait);
	}

	unlock
	System::dispatch();

	// Blocked Thread will return to this point in code after unblock
	// If it was woken up by signal operation return 0 else return 1
	if(System::running->wokenBySignal == 0)
		return 0;

	return 1;
#endif
}

// TODO KernelSem signal
int KernelSem::signal(int _n) {
#ifndef BCC_BLOCK_IGNORE
	if(_n < 0)
		return _n;
	else if(_n == 0)	// We should wake up just 1 Thread
		_n = 1;

	lock

	value += _n;		// Update value

	int numberOfThreadsToWakeUp = _n;

	int ret = 0;
	while(numberOfThreadsToWakeUp > 0 && waitList->getSize() > 0) {
		numberOfThreadsToWakeUp--;
		PCB *pcb = waitList->pop_front();
		pcb->wokenBySignal = 1;
		pcb->state = PCB::READY;
		Scheduler::put(pcb);
		ret++;		// Thread unblocked
	}

	Time tmp = 0;
	while(numberOfThreadsToWakeUp > 0 && timedWaitList->getSize() > 0) {
		numberOfThreadsToWakeUp--;
		tmp = timedWaitList->frontTime();		// Get wait time for unblocked pcb
		PCB *pcb = timedWaitList->pop_front();
		timedWaitList->addFrontTime(tmp);		// (No effect if called on empty list)
		pcb->wokenBySignal = 1;
		pcb->state = PCB::READY;
		Scheduler::put(pcb);
		ret++;		// Thread unblocked
	}

	unlock
	return ret;		// Return number of unblocked Threads
#endif
}

void KernelSem::tryToWakeUp() {
#ifndef BCC_BLOCK_IGNORE
//	lock

	if(timedWaitList->empty()) {
//		unlock
		return;
	}

	PCB *pcb = 0;

	timedWaitList->frontTick();

	if(timedWaitList->frontTime() == 0) {
		pcb = timedWaitList->pop_front();
		pcb->state = PCB::READY;
		pcb->wokenBySignal = 0;
		Scheduler::put(pcb);
		while(timedWaitList->frontTime() == 0) {
			pcb = timedWaitList->pop_front();
			pcb->state = PCB::READY;
			pcb->wokenBySignal = 0;
			Scheduler::put(pcb);
		}
	}

//	unlock
#endif
}	
