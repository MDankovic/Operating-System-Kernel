#include "IdleThr.h"
#include "Thread.h"

// Give minimum time slice to this thread
IdleThread::IdleThread() : Thread(defaultStackSize, 1) {}

void IdleThread::run() {
	while(1);
}

