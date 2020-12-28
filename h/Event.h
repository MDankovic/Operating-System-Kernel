#ifndef _EVENT_H_
#define _EVENT_H_

typedef unsigned char IVTNo;
class KernelEv;
#include "IVTEnt.h"


class Event
{
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal(); // can call KernelEv

private:
	KernelEv *myImpl;	// Isto kao za semaphore

};

// Interrupt routine generator
#define PREPAREENTRY(num,callOld)							\
	void interrupt interruptRoutine##num(...);				\
	IVTEntry newEntry##num(num, interruptRoutine##num);		\
	void interrupt interruptRoutine##num(...){				\
		newEntry##num.signal();								\
		if(callOld == 1)									\
			newEntry##num.callOldRoutine();					\
		dispatch();											\
	}
// VIDI DA LI TREBA DISPATCH

#endif /* _EVENT_H_ */
