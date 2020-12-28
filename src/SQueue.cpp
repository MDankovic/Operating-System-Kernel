#include "SQueue.h"

SQueue::Elem::Elem(SignalId _data, Elem *_next, Elem *_prev) : data(_data), next(_next), prev(_prev) {}
SQueue::Elem::~Elem() {}

// Constructor
SQueue::SQueue() : head(0), tail(0), size(0) {}

// Destructor
SQueue::~SQueue() {
	Elem *old = head;
	while(head != 0) {
		old = head;
		head = head->next;
		delete old;
	}
}

// Push new data to front
void SQueue::put(SignalId _signal) {
	Elem *newElem = new Elem(_signal, 0, 0);

	if(head == 0) {
		head = tail = newElem;
	} else {
		newElem->prev = tail;
		tail = tail->next = newElem;
	}

	size++;
}

// Get data from back
SignalId SQueue::pop_back() {
	if(head == 0)
		return 0;

	Elem *old = tail;
	SignalId ret = tail->data;

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
SignalId SQueue::pop_front() {
	if(head == 0)
		return 0;

	Elem *old = head;
	SignalId ret = head->data;

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
SignalId SQueue::peek_front() {
	if(head == 0)
		return 0;
	return head->data;
}

SignalId SQueue::peek_back() {
	if(head == 0)
		return 0;
	return tail->data;
}

// Delete all content
void SQueue::clear() {
	Elem *old = head;
	while(head != 0) {
		old = head;
		head = head->next;
		delete old;
	}

	head = tail = 0;
	size = 0;
}
