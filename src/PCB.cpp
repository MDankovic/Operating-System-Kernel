#include "PCB.h"
#include "Queue.h"
#include "SCHEDULE.H"
#include "System.h"
#include "Thread.h"
#include "HQueue.h"
#include "SQueue.h"

#include <dos.h>
#include <stdio.h>

// bcc.exe -mh -v -eOS.exe -IC:\bc31\include;..\; -LC:\bc31\lib;..\; ..\*.cpp  ..\*.lib

// Possible states of Thread
const int PCB::NEW = 0;
const int PCB::READY = 1;
const int PCB::RUNNING = 2;
const int PCB::BLOCKED = 3;
const int PCB::FINISHED = 4;

#ifdef SIGNAL
// Signal constants
const int PCB::SIG0 = 0;
const int PCB::SIG1 = 1;
const int PCB::SIG2 = 2;
const int PCB::SIG3 = 3;
const int PCB::SIG4 = 4;
const int PCB::SIG5 = 5;
const int PCB::SIG6 = 6;
const int PCB::SIG7 = 7;
const int PCB::SIG8 = 8;
const int PCB::SIG9 = 9;
const int PCB::SIG10 = 10;
const int PCB::SIG11 = 11;
const int PCB::SIG12 = 12;
const int PCB::SIG13 = 13;
const int PCB::SIG14 = 14;
const int PCB::SIG15 = 15;
const int PCB::NUMBER_OF_SIGNALS = 16;

int PCB::blockedSignalsGlobally[PCB::NUMBER_OF_SIGNALS] = { 0 };		// No blocked signals
#endif

// Mislim da je reseno, jer je pre poziva System::initialize promenljiva System::running == 0
// To znaci da kada pravim PCB za mainThread running ce biti 0, pa je onda parent == 0
PCB::PCB(Thread *_myThread, StackSize _stackSize, Time _timeSlice) : lockFlag(0) {
	lock
	stackSize = _stackSize > 65536 ? 65536 : _stackSize;	// Max stack size is 64KB
	stackSize = _stackSize / sizeof(unsigned);

	myThread = _myThread;

	timeSlice = _timeSlice;
	unlimited = timeSlice == 0 ? 1 : 0;

	id = VectorPCB::putPCB(this);		// Add PCB to global PCB vector

	// Dummy values
	stack = 0;
	ss = 0;
	sp = 0;
	bp = 0;
	wokenBySignal = 0;

	blockedThreads = new Queue();		// List of blocked Threads on this Thread

	state = NEW;

	// Mozda da za Queue napravim clone da ne prepisujem ovako sabanski
#ifdef SIGNAL
	parent = System::running;
	signalsReceived = new SQueue();				// Queue of received Signals
	for(int i = 0; i < NUMBER_OF_SIGNALS; i++) {
		signalHandlers[i] = new HQueue();		// SignalHandler Queue for every Signal type
		if(parent != 0) {
			blockedSignals[i] = parent->blockedSignals[i];			// Copy Signal blocked status from parent

			// Copy all handlers from parent to child
			int n = parent->signalHandlers[i]->getSize();
			for(int j = 0; j < n; j++) {
				SignalHandler tmp = parent->signalHandlers[i]->pop_front();	// Take from parent
				signalHandlers[i]->put(tmp);								// Add to child
				parent->signalHandlers[i]->put(tmp);						// Return back to parent
			}
		} else {
			blockedSignals[i] = 0;
		}
	}
#endif

	unlock
}

// Da li treba da se zakljuca?
PCB::~PCB() {
	//waitToComplete(System::running);
	lock
	if(blockedThreads != 0)
		delete blockedThreads;

	if(stack != 0)
		delete[] stack;

	VectorPCB::removePCB(id); 	// Delete from global list of PCBs

#ifdef SIGNAL
	if(signalsReceived != 0)
		delete signalsReceived;

	for(int i = 0; i < NUMBER_OF_SIGNALS; i++) {
		if(signalHandlers[i] != 0)
			delete signalHandlers[i];
	}
#endif

	unlock
}

// Make initial context (Poziva se iz vec zakljucane sekcije)
void PCB::makeStack() {
	stack = new unsigned[stackSize];

	stack[stackSize - 1] = 0x200;					// PSWI <= 1

#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 2] = FP_SEG(PCB::wrapper);	// Set PC to PCB::wrapper
	stack[stackSize - 3] = FP_OFF(PCB::wrapper);

	// -4..-11 ax, bx, cx, dx, es, ds, si, di can be random values for initial context
	// -12 represents bp

	ss = FP_SEG(stack + stackSize - 12);
	sp = FP_OFF(stack + stackSize - 12);
#endif

	bp = sp;	// Set Base Pointer
}

void PCB::start() {
	lock

	if(state != PCB::NEW) {			// Thread can be started only once
		unlock
		return;
	}
	System::threadCounter++;		// Increment number of active threads in system
	makeStack();
	state = PCB::READY;
	if(this != System::idleThread)
		Scheduler::put(this);

	unlock
}

void PCB::wrapper() {

	System::running->myThread->run();

	lock

	// processSignals();	// Thread finished, so it shouldn't be necessary to handle received signals

	System::running->state = PCB::FINISHED;		// Thread finished run method

	System::threadCounter--;					// Decrement number of active threads in system

	// Return all Threads blocked on this Thread to Scheduler
	while(!System::running->blockedThreads->empty()) {
		PCB *tmp = System::running->blockedThreads->pop_back();
		tmp->state = PCB::READY;
		Scheduler::put(tmp);
	}

#ifdef SIGNAL
	// Signal parent Thread that I'm finished
	if(System::running->parent != 0) {
		System::running->signal(SIG1);
	}

	System::running->signal(SIG2);		// Signal to myself that I'm finished
#endif

	unlock
	System::dispatch();		// Demand context switch
}


void PCB::waitToComplete(PCB *_pcb) {
	lock

	// Can't wait on FINISHED thread
	if(this->state == PCB::FINISHED) {
		unlock
		return;
	}

	// Can't wait on myself
	if(this == _pcb) {
		unlock
		return;
	}

	// Can't wait on idleThread
	if(this == System::idleThread) {
		unlock
		return;
	}

	// Safe to block on this thread
	_pcb->state = PCB::BLOCKED;
	blockedThreads->put(_pcb);
	unlock

	// Context switch on demand
	System::dispatch();
}

void PCB::setID(ID _id) {
	id = _id;
}

int PCB::getID() {
	return id;
}

void PCB::setState(int _state) {
	state = _state;
}

int PCB::getState() {
	return state;
}

#ifdef SIGNAL
void PCB::signal(SignalId signal) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		signalsReceived->put(signal);
	}
	unlock
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		signalHandlers[signal]->put(handler);
	}
	unlock
}

void PCB::unregisterAllHandlers(SignalId id) {
	lock
	// SIG0 should not be unregistered
	// Simple change to id > 0 will solve that restriction
	if(id >= 0 && id < PCB::NUMBER_OF_SIGNALS) {
		signalHandlers[id]->clear();
	}
	unlock
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	lock
	if(id >= 0 && id < PCB::NUMBER_OF_SIGNALS) {
		signalHandlers[id]->swap(hand1, hand2);
	}
	unlock
}

void PCB::blockSignal(SignalId signal) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		blockedSignals[signal] = 1;
	}
	unlock
}

// Static method
void PCB::blockSignalGlobally(SignalId signal) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		PCB::blockedSignalsGlobally[signal] = 1;
	}
	unlock
}

void PCB::unblockSignal(SignalId signal) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		blockedSignals[signal] = 0;
	}
	unlock
}

// Static method
void PCB::unblockSignalGlobally(SignalId signal) {
	lock
	if(signal >= 0 && signal < PCB::NUMBER_OF_SIGNALS) {
		blockedSignalsGlobally[signal] = 0;
	}
	unlock
}

// Called from locked section
void PCB::processSignals() {
	for(int i = signalsReceived->getSize(); i > 0; i--) {
		SignalId signal = signalsReceived->pop_front();
		if(blockedSignals[signal] == 1 || blockedSignalsGlobally[signal] == 1) {
			// Signal can be dumped, but we can also handle it later when it's not blocked
			// Comment line below to dump blocked signals during signal processing time
			signalsReceived->put(signal);
			continue;
		}


		for(int j = signalHandlers[signal]->getSize(); j > 0; j--) {
			SignalHandler handler = signalHandlers[signal]->pop_front();
			signalHandlers[signal]->put(handler);

			handler();
		}

		// If Thread received SIG0 stop with signal processing and finish the thread
		if(signal == PCB::SIG0) {
			break;
		}

		// All threads have default SIG0 handler in their signalHandlers[0] queue
	}

}

void PCB::sig0Handler() {
	System::running->state = FINISHED;

	System::threadCounter--;		// Decrement number of active threads in system

	// Return all Threads blocked on this Thread to Scheduler
	while(!System::running->blockedThreads->empty()) {
		PCB *tmp = System::running->blockedThreads->pop_back();
		tmp->state = PCB::READY;
		Scheduler::put(tmp);
	}
}
#endif





