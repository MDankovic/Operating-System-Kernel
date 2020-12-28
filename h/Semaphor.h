#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

typedef unsigned int Time;

class KernelSem;

class Semaphore
{
public:
	Semaphore(int init = 1);
	virtual ~Semaphore();

	virtual int wait(Time maxTimeToWait = 0);	// Ne bi trebalo da ima podrazumevanu vrednost, ali lakse je nego da svaki put zovemo sa 0
	virtual int signal(int n = 0);

	int val() const;

protected:

private:
	KernelSem *myImpl;	// my implementation
};


#endif /* _SEMAPHORE_H_ */
