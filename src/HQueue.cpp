#include "HQueue.h"
#include "Thread.h"


HQueue::Elem::Elem(SignalHandler _data, Elem *_next, Elem *_prev) : data(_data), next(_next), prev(_prev) {}
HQueue::Elem::~Elem() {}

// Constructor
HQueue::HQueue() : head(0), tail(0), size(0) {}

// Destructor
HQueue::~HQueue() {
	Elem *old = head;
	while(head != 0) {
		old = head;
		head = head->next;
		delete old;
	}
}

// Push new data to front
void HQueue::put(SignalHandler _handler) {
	Elem *newElem = new Elem(_handler, 0, 0);

	if(head == 0) {
		head = tail = newElem;
	} else {
		newElem->prev = tail;
		tail = tail->next = newElem;
	}

	size++;
}

// Get data from back
SignalHandler HQueue::pop_back() {
	if(head == 0)
		return 0;

	Elem *old = tail;
	SignalHandler ret = tail->data;

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
SignalHandler HQueue::pop_front() {
	if(head == 0)
		return 0;

	Elem *old = head;
	SignalHandler ret = head->data;

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
SignalHandler HQueue::peek_front() {
	if(head == 0)
		return 0;
	return head->data;
}

SignalHandler HQueue::peek_back() {
	if(head == 0)
		return 0;
	return tail->data;
}

// Delete all content
void HQueue::clear() {
	Elem *old = head;
	while(head != 0) {
		old = head;
		head = head->next;
		delete old;
	}

	head = tail = 0;
	size = 0;
}

// Swap, specially for handlers
void HQueue::swap(SignalHandler hand1, SignalHandler hand2) {
	// Find position of hand1
	Elem *h1Elem = head;
	while(h1Elem != 0 && h1Elem->data != hand1) {
		h1Elem = h1Elem->next;
	}

	if(h1Elem == 0)
		return;

	// Find position of hand2
	Elem *h2Elem = head;
	while(h2Elem != 0 && h2Elem->data != hand1) {
		h2Elem = h2Elem->next;
	}

	if(h2Elem == 0)
		return;

	// Just set new handlers
	h1Elem->data = hand2;
	h2Elem->data = hand1;
}
