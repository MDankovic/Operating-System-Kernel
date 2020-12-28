#ifndef _IVTENT_H_
#define _IVTENT_H_

#include "Event.h"

class KernelEv;

typedef void interrupt (*pInterrupt)(...);

class IVTEntry {
public:
	IVTEntry(IVTNo _ivtNo, pInterrupt _newRoutine);
	~IVTEntry();

	static IVTEntry* getIVTEntry(IVTNo _ivtNo);

	void callOldRoutine();

	void signal();

protected:
	friend class System;
	friend class KernelEv;

private:
	IVTNo ivtNo;
	pInterrupt oldRoutine;
	int callOld;			// Call old interrupt routine flag
	KernelEv *myEvent;
};

#endif /* _IVTENT_H_ */
