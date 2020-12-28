#ifndef _THREAD_H_
#define _THREAD_H_

#define SIGNAL

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; 				// time, x 55ms
const Time defaultTimeSlice = 2; 		// default = 2*55ms
typedef int ID;
class PCB;

#ifdef SIGNAL
typedef void (*SignalHandler)();
typedef unsigned SignalId;
#endif

class Thread
{
public:
	void start();
	void waitToComplete();	// PISE U MATERIJALIMA STRANA 6
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID id);
//	static void sleep(Time timeToSleep);

#ifdef SIGNAL
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
	friend class PCB;
	friend class System;

	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {}

private:
	PCB *myPCB;				// Kernel PCB object for this Thread
};

void dispatch();	// Global method

#endif /* _THREAD_H_ */
