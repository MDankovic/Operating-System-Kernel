#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#include "Thread.h"

class UserThread : public Thread {
public:
	UserThread(int _argc, char** _argv);
	virtual ~UserThread();
	virtual void run();

	int getRet();

private:
	int argc;
	char **argv;
	int ret;
};



#endif 	/* _UTHREAD_H_ */
