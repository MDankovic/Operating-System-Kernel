#ifndef _KERNELSEM_H_
#define _KERNELSEM_H_

#include "Thread.h"

class Queue;
class Semaphore;
class TimerQueue;

class KernelSem
{
public:
	KernelSem(int _init = 1, Semaphore *_mySemaphore);
	virtual ~KernelSem();

	virtual int wait(Time _maxTimeToWait = 0);
	virtual int signal(int _n = 0);

	void tryToWakeUp();

	int val() const;

protected:
	friend class System;

private:
	int id;

	volatile int value;
	Queue *waitList;
	TimerQueue *timedWaitList;

	Semaphore *mySemaphore;
};

#endif /* _KERNELSEM_H_ */
