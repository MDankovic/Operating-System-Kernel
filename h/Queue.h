#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Thread.h"

class PCB;

class Queue
{
protected:
	struct Elem {
		PCB *data;
		Elem *next;
		Elem *prev;
		Time maxTimeToWait;
		Elem(PCB *_data, Elem *_next = 0, Elem *_prev = 0, Time _maxTimeToWait = 0);
		~Elem();
	};

public:
	Queue();
	virtual ~Queue();

	virtual void put(PCB *_pcb, Time _maxTimeToWait = 0);

	PCB* pop_back();
	PCB* pop_front();
	PCB* peek_front();
	PCB* peek_back();

	int getSize() { return size; }
	int empty() { return size == 0; }

	Queue *clone();

protected:
	Elem *head;
	Elem *tail;
	int size;

};

#endif /* _QUEUE_H_ */
