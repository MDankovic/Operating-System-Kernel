#ifndef _HQUEUE_H_
#define _HQUEUE_H_

#include "Thread.h"

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class HQueue
{
protected:
	struct Elem {
		SignalHandler data;
		Elem *next;
		Elem *prev;
		Elem(SignalHandler _data, Elem *_next = 0, Elem *_prev = 0);
		~Elem();
	};

public:
	HQueue();
	virtual ~HQueue();

	virtual void put(SignalHandler _handler);

	SignalHandler pop_back();
	SignalHandler pop_front();
	SignalHandler peek_front();
	SignalHandler peek_back();

	void clear();

	void swap(SignalHandler hand1, SignalHandler hand2);

	int getSize() { return size; }
	int empty() { return size == 0; }

protected:
	Elem *head;
	Elem *tail;
	int size;

};


#endif /* _HQUEUE_H_ */
