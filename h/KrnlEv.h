#ifndef _KRNLEV_H_
#define _KRNLEV_H_

#include "Event.h"



class KernelEv {
public:
	KernelEv(IVTNo ivt_number, Event *_myEvent);
	~KernelEv();

	void wait();
	void signal();

protected:
	friend class System;
	friend class PCB;
	friend class Thread;
	friend class Event;

private:
	IVTNo ivtNo;		// Interrupt routine nuber
	Event *myEvent;		// Event represented by this KernelEv
	PCB *owner;			// Creator Thread of this Event

	PCB *blocked;
	int value;
};




#endif /* KRNLEV_H_ */
