#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "Thread.h"

class PCB;
class Thread;
class IdleThread;
class TimerQueue;
class IVTEntry;

#define NUMBER_OF_IVT_ENTRIES 256

typedef void interrupt (*pInterrupt)(...);

// Disable interrupts (Push PSW to stack)
#define hardLock asm{\
	pushf;\
	cli;\
}

// Pop PSW from stack
#define hardUnlock {asm {popf}}

// 0 UNLOCKED
// > 0 LOCKED
#define lock {System::running->lockFlag++;}

#define unlock {System::running->lockFlag--;		\
	if(System::running->lockFlag < 0)				\
		System::running->lockFlag = 0;				\
	if(System::dispatchRequested)					\
		dispatch();									\
}

// Will be defined
extern void tick();

class System {
public:

	static void dispatch();
//	static void sleep(PCB *_pcb, Time _timeToSleep);	// Not used, implemented and commented

	static void System::initialize();	// Should be called before everything
	static void System::restore();		// Sould be called before exiting program

	static volatile int lockFlag;		// SoftLock flag used for signals for interrupts without preemption

protected:
	friend class PCB;
	friend class Thread;
	friend class KernelSem;
	friend class KernelEv;
	friend class IVTEntry;

private:
	static int dispatchRequested;		//	Context switch on demand flag
	static int counter;					//	Global timer
	static int threadCounter;			// 	Active Thread counter
	static PCB *running;				//	Global pointer to currently running Thread
	static PCB *idleThread;				// 	Global pointer to idleThread
	static PCB *mainThread;				//	Global pointer to starting Thread
	static IVTEntry *ivtEntries[NUMBER_OF_IVT_ENTRIES];	// Array of IVT Entries

	static TimerQueue *sleepList;		// Global sleeping Threads list NOT USED
	static void timerTick();

	static void wakeUpThreads();

	// Helper methods to initialize and restore system
	static void initializeIVT();
	static void restoreIVT();
	static void initializeVectors();

	static void interrupt timer(...);
	static pInterrupt oldTimer;
};

/*------------------VECTOR PCB------------------*/
class VectorPCB
{
public:
	static void removePCB(int _id);
	static PCB *getPCB(int _id);
	static int putPCB(PCB *_pcb);

protected:
	static PCB **vector;
	static void resize();

	static int capacity;
	static int count;

private:
	friend class System;
};

/*---------------VECTOR SEMAPHORE---------------*/
class VectorSemaphore
{
public:
	static void removeSemaphore(int _id);
	static KernelSem *getSemaphore(int _id);
	static int putSemaphore(KernelSem *_kSem);

protected:
	static KernelSem **vector;
	static void resize();

	static int capacity;
	static int count;

private:
	friend class System;
};

#endif /* _SYSTEM_H_ */
