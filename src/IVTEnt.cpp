#include "IVTEnt.h"
#include "System.h"
#include "KrnlEv.h"
#include <dos.h>

IVTEntry::IVTEntry(IVTNo _ivtNo, pInterrupt _newRoutine) : ivtNo(_ivtNo), myEvent(0) {
	hardLock

	oldRoutine = getvect(ivtNo);		// Save old interrupt routine
	setvect(ivtNo, _newRoutine);		// Set new interrupt routine
	System::ivtEntries[ivtNo] = this;	// Connect global array to this object

	hardUnlock
}

IVTEntry::~IVTEntry() {
	hardLock

	setvect(ivtNo, oldRoutine);		// Return old interrupt routine
	System::ivtEntries[ivtNo] = 0;	// Free space in global IVTEntry array

	hardUnlock
}

IVTEntry* IVTEntry::getIVTEntry(IVTNo _ivtNo) {
	return System::ivtEntries[_ivtNo];
}

void IVTEntry::callOldRoutine() {
	oldRoutine();
}

void IVTEntry::signal() {
	if(myEvent != 0)
		myEvent->signal();
}

