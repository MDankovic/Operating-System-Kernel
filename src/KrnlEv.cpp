#include "KrnlEv.h"
#include "PCB.h"
#include "System.h"
#include "SCHEDULE.H"
#include "Thread.h"
#include "IVTEnt.h"

// Connect Event to system's global event array
KernelEv::KernelEv(IVTNo _ivtNo, Event *_myEvent) : ivtNo(_ivtNo), myEvent(_myEvent), value(0), blocked(0) {
	lock
	owner = System::running;
	System::ivtEntries[ivtNo]->myEvent = this;
	unlock
}

// Remove Event from system's global event array
KernelEv::~KernelEv() {
	lock
	System::ivtEntries[ivtNo]->myEvent = 0;
	unlock
}

// Block owner on Event
void KernelEv::wait() {
	lock
	if(owner == System::running) {			// If running is not owner, operation has no effect
		if(value == 0) {					
			owner->state = PCB::BLOCKED;	// Block owner on this event
			blocked = owner;
			unlock
			dispatch();
		} else {
			value = 0;
			unlock
		}
	} else {
		unlock	
	}
}

// Release owner from Event
void KernelEv::signal() {
	lock
	if(blocked == 0) {
		value = 1;
	} else {
		blocked = 0;
		value = 0;
		owner->state = PCB::READY;
		Scheduler::put(owner);
	}
	unlock
}
