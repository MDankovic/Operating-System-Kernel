#ifndef _SQUEUE_H_
#define _SQUEUE_H_

#include "Thread.h"

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class SQueue
{
protected:
	struct Elem {
		SignalId data;
		Elem *next;
		Elem *prev;
		Elem(SignalId _data, Elem *_next = 0, Elem *_prev = 0);
		~Elem();
	};

public:
	SQueue();
	virtual ~SQueue();

	virtual void put(SignalId _signal);

	SignalId pop_back();
	SignalId pop_front();
	SignalId peek_front();
	SignalId peek_back();

	void clear();

	int getSize() { return size; }
	int empty() { return size == 0; }

protected:
	Elem *head;
	Elem *tail;
	int size;

};



#endif /* _SQUEUE_H_ */
