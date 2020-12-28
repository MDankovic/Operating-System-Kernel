#ifndef _IDLETHR_H_
#define _IDLETHR_H_

#include "Thread.h"

//Placeholder Thread when there is no READY Threads in Scheduler
class IdleThread : public Thread {
public:
	IdleThread();
	virtual void run();
	void start();

protected:

private:

};

#endif	/* _IDLETHR_H_ */
