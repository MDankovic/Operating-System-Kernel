#include "IdleThr.h"
#include "PCB.h"
#include "System.h"
#include "SCHEDULE.H"
#include "KrnlSem.h"
#include "Queue.h"
#include "TQueue.h"

#include <stdio.h>
#include <iostream.h>
#include <dos.h>

// Helper variables for context switch
unsigned tss;
unsigned tsp;
unsigned tbp;

int System::dispatchRequested = 0;
int System::counter = 0;
int System::threadCounter = 0;
PCB *System::running = 0;
PCB *System::idleThread = 0; 
PCB *System::mainThread = 0; 
volatile int System::lockFlag = 1;			// NOT USED
pInterrupt System::oldTimer = 0;
TimerQueue *System::sleepList = 0;			// NOT USED
IVTEntry *System::ivtEntries[NUMBER_OF_IVT_ENTRIES] = { 0 };


// New interrupt routine
void interrupt System::timer(...) {
	if (!dispatchRequested)	{			// System timer interrupt
		oldTimer();						// Call oldTimer if timer interrupt really happened		//asm int 60h;
		counter = (counter == 0 ? 0 : counter - 1);
		wakeUpThreads();				// Wake up Threads timed waiting on Semaphores
		tick();
	}


	if ((counter == 0 && !running->unlimited) || dispatchRequested) {	// time exceeded or context switch demanded
		//printf("Proverava running->lockFlag");
		if(running != 0 && running->lockFlag == 0 || dispatchRequested) {
			//printf(". ");
			dispatchRequested = 0;

#ifndef BCC_BLOCK_IGNORE
				// Save stack
			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
#endif
			// Save sp, ss, bp
			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;
			if((running->state == PCB::RUNNING || running->state == PCB::READY) && running != idleThread) {
				running->state = PCB::READY;
				Scheduler::put(running);
			}

			// Take new Thread, handle signals
			// If Thread received SIG0, take new Thread and repeat the process
			do {
				running = Scheduler::get();		// get new running Thread

				if(running == 0) {
					if(threadCounter == 1)
						running = mainThread;
					else {
						running = idleThread;
					}
				}

				// Prepare new context
				tsp = running->sp;
				tss = running->ss;
				tbp = running->bp;

				counter = running->timeSlice;
				running->state = PCB::RUNNING;

#ifndef BCC_BLOCK_IGNORE
				// Restore stack
			asm{
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
#endif

				// do while je zbog obrade signala, jer ako je stigao signal 0, onda je moja nit zavrsila pa necu
				// Da izadjem iz timera nego hocu da uyzmem novu nit
#ifdef SIGNAL
			running->processSignals();
#endif
			} while(running->state == PCB::FINISHED);

		}	// END IF LOCKFLAG
		else {
			dispatchRequested = 1;
		}

	}

	//dispatchRequested = 0;	// Delete context switch on demand flag
}

// Prepare for context switch
void System::dispatch() {
#ifndef BCC_BLOCK_IGNORE
	hardLock
	dispatchRequested = 1;
	timer();
	hardUnlock
#endif
}


/*
 * mainThread timeSlice
 */
void System::initialize() {
#ifndef BCC_BLOCK_IGNORE
	hardLock

	initializeIVT();
	initializeVectors();

	mainThread = new PCB(0, defaultStackSize, 2);
	running = mainThread;
	VectorPCB::removePCB(mainThread->getID()); 		// Za mainThread i ne mora ovako ali neka ga za sada
	//threadCounter--;								// Ne zelimo da main bude u active threads
	mainThread->setID(-1);
	mainThread->state = PCB::RUNNING;
	mainThread->registerHandler(0, PCB::sig0Handler);

	Thread *idleT = new IdleThread();
	idleThread = VectorPCB::getPCB(idleT->getId());
	idleThread->registerHandler(0, PCB::sig0Handler);
	idleT->start();

	hardUnlock
#endif
}

void System::restore() {
#ifndef BCC_BLOCK_IGNORE
	hardLock

	restoreIVT();

	delete idleThread->myThread;
	delete mainThread;
	delete VectorPCB::vector;
	delete VectorSemaphore::vector;

	hardUnlock
#endif
}

void System::initializeIVT() {
#ifndef BCC_BLOCK_IGNORE
	oldTimer = getvect(0x8);
	setvect(0x8, timer);		// Set my timer  0x08
	setvect(0x60, oldTimer);	// Set old timer 0x60
#endif
}

void System::restoreIVT() {
#ifndef BCC_BLOCK_IGNORE
	setvect(0x8, oldTimer);		// Restore old timer 0x08
#endif
}

// Record of created Threads and Semaphores
void System::initializeVectors() {
	VectorSemaphore::capacity = 50;
	VectorSemaphore::vector = new KernelSem*[VectorSemaphore::capacity];
	for(int i = 0; i < VectorSemaphore::capacity; i++) {
		VectorSemaphore::vector[i] = 0;
	}

	VectorPCB::capacity = 50;
	VectorPCB::vector = new PCB*[VectorPCB::capacity];
	for(int j = 0; j < VectorPCB::capacity; j++) {
		VectorPCB::vector[j] = 0;
	}
}

// Go through all semaphores and try to wake up time blocked threads
void System::wakeUpThreads() {
	for(int i = 0; i < VectorSemaphore::capacity; i++) {
		if(VectorSemaphore::vector[i] == 0) {
			continue;
		}
		VectorSemaphore::vector[i]->tryToWakeUp();
	}
}

/*
// TODO UNUSED should be deleted
void System::sleep(PCB *_pcb, Time _timeToSleep) {
	lock
	if(_timeToSleep) {
		unlock
		return;
	}

	_pcb->state = PCB::BLOCKED;
	sleepList->put(_pcb, _timeToSleep);
	unlock

	dispatch();
}

void System::timerTick() {
	lock
	if(!sleepList->empty()) {
		PCB *pcb = 0;

		sleepList->frontTick();

		if(sleepList->frontTime() == 0) {
			pcb = sleepList->pop_front();
			pcb->state = PCB::READY;
			Scheduler::put(pcb);

			while(sleepList->frontTime() == 0) {
				pcb = sleepList->pop_front();
				pcb->state = PCB::READY;
				Scheduler::put(pcb);
			}
		}
	}

	unlock

}
*/

/*------------------VECTOR PCB------------------*/

// Initialization of static variables
PCB** VectorPCB::vector = 0;
int VectorPCB::capacity = 0;
int VectorPCB::count = 0;

// Resize vector by factor 2
void VectorPCB::resize() {
	PCB **tmp = vector;

	vector = new PCB*[capacity * 2];

	for(int i = 0; i < capacity; i++) {
		vector[i] = tmp[i];
	}

	for(int j = capacity; j < 2*capacity; j++) {
		vector[j] = 0;
	}

	delete tmp;		// Free old vector
	capacity *= 2;
}

// Put new PCB in vector
int VectorPCB::putPCB(PCB *_pcb) {
	if(count == capacity)
		resize();

	for(int i = 0; i < capacity; i++) {
		if(vector[i] == 0) {
			_pcb->setID(i);		// save PCB ID
			vector[i] = _pcb;
			count++;			// new element added
			return i;
		}
	}

	return 0;
}

// Get PCB with given ID
PCB* VectorPCB::getPCB(int _id) {
	if(_id < 0 || _id >= capacity)
		return 0;
	return vector[_id];
}

// Remove PCB from vector
void VectorPCB::removePCB(int _id) {
	if(_id < 0 || _id >= capacity)
		return;
	vector[_id] = 0;
	count--;					// element deleted
}


/*---------------VECTOR SEMAPHORE---------------*/

// Initialization of static variables
KernelSem** VectorSemaphore::vector = 0;
int VectorSemaphore::capacity = 0;
int VectorSemaphore::count = 0;

// Resize vector by factor 2
void VectorSemaphore::resize() {
	KernelSem **tmp = vector;

	vector = new KernelSem*[capacity * 2];

	for(int i = 0; i < capacity; i++) {
		vector[i] = tmp[i];
	}

	for(int j = capacity; j < 2*capacity; j++) {
		vector[j] = 0;
	}

	delete tmp;		// Free old vector
	capacity *= 2;
}

// Put new KernelSem in vector
int VectorSemaphore::putSemaphore(KernelSem *_kSem) {
	if(count == capacity)
		resize();

	for(int i = 0; i < capacity; i++) {
		if(vector[i] == 0) {
			vector[i] = _kSem;
			count++;				// new element added
			return i;
		}
	}

	return 0;
}

// Get KernelSem with given ID
KernelSem* VectorSemaphore::getSemaphore(int _id) {
	if(_id < 0 || _id >= capacity)
			return 0;
		return vector[_id];
}

// Remove KernelSem from vector
void VectorSemaphore::removeSemaphore(int _id) {
	vector[_id] = 0;
	count--;					// element deleted
}









