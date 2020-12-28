#include "Queue.h"

Queue::Elem::Elem(PCB *_data, Elem *_next, Elem *_prev, Time _maxTimeToWait) : data(_data), next(_next), prev(_prev), maxTimeToWait(_maxTimeToWait) {}
Queue::Elem::~Elem() {}

// Constructor
Queue::Queue() : head(0), tail(0), size(0) {}

// Destructor
Queue::~Queue() {
	Elem *old = head;
	while(head != 0) {
		old = head;
		head = head->next;
		delete old;
	}
}

// Push new data to front
void Queue::put(PCB *_pcb, Time _maxTimeToWait) {
	Elem *newElem = new Elem(_pcb, 0, 0, _maxTimeToWait);

	if(head == 0) {
		head = tail = newElem;
	} else {
		newElem->prev = tail;
		tail = tail->next = newElem;
	}

	size++;
}

// Get data from back
PCB * Queue::pop_back() {
	if(head == 0)
		return 0;

	Elem *old = tail;
	PCB *ret = tail->data;

	size--;
	if(size == 0) {
		head = tail = 0;
	} else {
		tail = tail->prev;
		tail->next = 0;
	}

	delete old;
	return ret;
}

// Get data from front
PCB * Queue::pop_front() {
	if(head == 0)
		return 0;

	Elem *old = head;
	PCB *ret = head->data;

	size--;
	if(size == 0) {
		head = tail = 0;
	} else {
		head = head->next;
		head->prev = 0;
	}

	delete old;
	return ret;
}

// Get data from front
PCB * Queue::peek_front() {
	if(head == 0)
		return 0;
	return head->data;
}

PCB * Queue::peek_back() {
	if(head == 0)
		return 0;
	return tail->data;
}

Queue *Queue::clone() {
	Queue *newQueue = new Queue();

	Elem *curr = head;
	while(curr != 0) {
		newQueue->put(curr->data, curr->maxTimeToWait);
		curr = curr->next;
	}

	return newQueue;
}


