#ifndef _PCB_H_
#define _PCB_H_

#include "Thread.h"

#define SIGNAL

class Queue;
class SQueue;
class HQueue;
class System;
class IVTEntry;

class PCB
{
public:
	// Possible states of Thread
	static const int NEW;
	static const int READY;
	static const int RUNNING;
	static const int BLOCKED;
	static const int FINISHED;


	int lockFlag;		// Soft Lock

	// Constructor and destructor
	PCB(Thread *_myThread, StackSize _stackSize = defaultStackSize, Time _timeSlice = defaultTimeSlice);
	virtual ~PCB();

	// Helper method for making starting stack
	void makeStack();

	// ID setter and getter
	void setID(ID _id);
	ID getID();

	// State setter and getter
	int getState();
	void setState(int _state);

	// Start Thread
	void start();

	void waitToComplete(PCB *_pcb);

#ifdef SIGNAL
	// Signal constants
	static const int SIG0;
	static const int SIG1;
	static const int SIG2;
	static const int SIG3;
	static const int SIG4;
	static const int SIG5;
	static const int SIG6;
	static const int SIG7;
	static const int SIG8;
	static const int SIG9;
	static const int SIG10;
	static const int SIG11;
	static const int SIG12;
	static const int SIG13;
	static const int SIG14;
	static const int SIG15;
	static const int NUMBER_OF_SIGNALS;

    void signal(SignalId signal);

    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId id);
    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

    void blockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);
    void unblockSignal(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);
#endif

protected:
	friend class VectorPCB;
	friend class VectorSemaphore;
	friend class System;
	friend class Thread;
	friend class KernelSem;
	friend class Semaphore;
	friend class KernelEv;
	friend class Event;
	friend class IVTEntry;

private:
	static void wrapper();

	unsigned *stack;		// Array that represents Thread stack
	StackSize stackSize;	// Stack capacity (max 65536)

	unsigned ss;		// Stack segment
	unsigned sp;		// Stack offset
	unsigned bp;		// Base pointer
	unsigned timeSlice;	// timeSlice * 55ms
	int state;			// State of Thread
	int unlimited;		// Unlimited time slice flag
	ID id;				// ID of this Thread

	int wokenBySignal;	// Woken by Semaphore signal flag

	Queue *blockedThreads;	// List of blocked Threads on this Thread
	Thread *myThread;		// Thread represented by this PCB

#ifdef SIGNAL
	PCB *parent;												// Parent Thread
	SQueue *signalsReceived;									// Received signals ready for handling
	HQueue *signalHandlers[16];					// Array of Queues for SignalHandlers
	int blockedSignals[16];						// Signals blocked for this Thread
	static int blockedSignalsGlobally[16];		// Globally blocked Signals

	void processSignals();		// Called when it's time to handle received signals

	static void sig0Handler();	// Signal 0 handler
#endif
};

#endif /* _PCB_H_ */
