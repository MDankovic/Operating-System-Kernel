#include "UThread.h"

extern int userMain(int argc, char* argv[]);

UserThread::UserThread(int _argc, char* _argv[]) : Thread(defaultStackSize, 2) {
	argc = _argc;
	argv = _argv;
	ret = 100;
}


UserThread::~UserThread() {
	waitToComplete();
}

void UserThread::run() {
	ret = userMain(argc, argv);
}

int UserThread::getRet() {
	return ret;
}
