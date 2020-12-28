#include "Thread.h"
#include "UThread.h"
#include "System.h"
#include "TQueue.h"
#include "Semaphor.h"
#include "Event.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <DOS.H>
#include <STDARG.H>

// bcc.exe -mh -v -eOS.exe -IC:\bc31\include;..\; -LC:\bc31\lib;..\; ..\*.cpp  ..\*.lib

extern int userMain(int argc, char *argv[]);

int main(int argc, char *argv[]) {

	System::initialize();

	UserThread *ut = new UserThread(argc, argv);
	ut->start();
	ut->waitToComplete();
	int ret = ut->getRet();
	delete ut;

	System::restore();


	return ret;
}

/*
int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	lock
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlock
		return res;
}
*/

/*
	A *a1 = new A(2, 1);
	A *a2 = new A(2, 2);
	A *a3 = new A(2, 3);

	a1->start();
	a2->start();
	a3->start();

	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 30000; j++) {
			for(int k = 0; k < 30000; k++){
			}
		}

		printf("main %d\n", i);
	}

	delete a3;
	delete a2;
	delete a1;
*/


/*
	sem = new Semaphore(0);

	for(int l = 0; l < THR; l++) {
		thrArr[l] = new A(l % 3 + 1, l);
	}

	for(l = 0; l < THR; l++) {
		thrArr[l]->start();
	}

	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 30000; j++) {
			for(int k = 0; k < 30000; k++){
			}
		}
		printf("main %d\n", i);
	}

	for(l = 0; l < THR; l++) {
		delete thrArr[l];
	}
*/

/*
	Queue *q = new Queue();
	TimerQueue *tq = new TimerQueue();

	q->put(0, 1);
	q->put(0, 2);
	q->put(0, 3);
	q->put(0, 4);
	q->put(0, 5);
	q->put(0, 6);
	q->put(0, 7);
	q->put(0, 8);
	q->put(0, 9);

	for(int i = 0; i < 9; i++) {
		printf("%d ", q->getSize());
		if(i % 2)
			q->pop_front();
		else
			q->pop_back();
	}
	printf("\nempty: %d\nsize: %d\n", q->empty(), q->getSize());


	tq->put(0, 1);
	tq->put(0, 2);
	tq->put(0, 4);
	tq->put(0, 7);
	tq->put(0, 11);
	tq->put(0, 16);
	tq->put(0, 22);
	tq->put(0, 29);
	tq->put(0, 37);

	for(i = 0; i < 9; i++) {
		printf("%d ", tq->frontTime());
		tq->pop_front();
	}
	printf("\nempty: %d\nsize: %d\n", tq->empty(), tq->getSize());

	delete q;
	delete tq;
*/

/*
	TimerQueue *tq = new TimerQueue();

	tq->put(0, 5);
	tq->put(0, 6);
	tq->put(0, 8);
	tq->put(0, 9);
	tq->put(0, 15);
	tq->put(0, 12);
	tq->put(0, 2);

	while(!tq->empty()) {
		printf("%d ", tq->getHeadTime());
		tq->pop_front();
	}

	delete tq; 
*/

/*
 	List<int> list;

	list.push_front(4);
	list.push_front(4);
	list.push_front(5);
	list.push_front(6);
	list.push_front(7);
	list.push_front(8);

	for(List<int>::Iterator it = list.begin(); it != list.end(); it++)
		printf("%d\n", *it);

	printf("%d\n", list.empty());
	printf("%d\n", list.getSize());

	List<double> listC;

	listC.push_front(1.45);
	listC.push_front(1.312);
	listC.push_front(1.34);
	listC.push_front(1.33);
	listC.push_front(1.23);

	for(List<double>::Iterator dit = listC.begin(); dit != listC.end(); dit++)
		printf("%f\n", *dit);

	printf("%d\n", listC.empty());
	printf("%d", listC.getSize());
 */

/*
class A;
class B;

A *a;
B *b;
Semaphore *sem;

#define THR 9

A* thrArr[THR];
Semaphore *semArr[THR];


class A : public Thread {
private:
	int num;

public:
	A(Time timeSlice, int num = 0) : Thread(defaultStackSize, timeSlice), num(num) {}

	virtual void run() {
		printf("run %d\n", num);
		for(int i = 0; i < 10; i++){
			for(int j = 0; j < 10000; j++) {
				for(int k = 0; k < 10000; k++){
				}
			}
			printf("A%d %d\n", num, i);
		}

	}

	~A() {
		waitToComplete();
	}

};

class B : public Thread {
public:
	B(Time timeSlice) : Thread(defaultStackSize, timeSlice) {}
	void run() {
		for(int i = 0; i < 10; i++){
			for(int j = 0; j < 30000; j++) {
				for(int k = 0; k < 30000; k++){
				}
			}
			printf("B %d\n", i);
		}
		printf("B zavrsio\n");
	}

	~B() {
		waitToComplete();
	}
};

extern int userMain(int argc, char *argv[]);
 */
