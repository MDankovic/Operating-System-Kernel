#include "TQueue.h"

/*
 * Transform this: 5	6	8	9	13	13	20
 * Into this:	   5	1	2	1	4	0	7
 */
void TimerQueue::put(PCB *_pcb, Time _maxTimeToWait) {
	Elem *newElem = new Elem(_pcb, 0, 0, _maxTimeToWait);

	if(head == 0) {
		head = tail = newElem;
	} else {
		Elem *curr = head;
		Elem *prev = 0;
		while(curr != 0 && curr->maxTimeToWait < newElem->maxTimeToWait) {
			newElem->maxTimeToWait -= curr->maxTimeToWait;
			prev = curr;
			curr = curr->next;
		}

		newElem->next = curr;
		newElem->prev = prev;

		// Put to front
		if(prev == 0)
			head = newElem;
		else
			prev->next = newElem;

		// Put to back
		if(curr == 0)
			tail = newElem;
		else
			curr->prev = newElem;

		if(newElem->next != 0)
			newElem->next->maxTimeToWait -= newElem->maxTimeToWait;

	}

	size++;
}

void TimerQueue::addFrontTime(Time _timeToAdd) {
	if(head == 0)
		return;

	head->maxTimeToWait += _timeToAdd;
}

void TimerQueue::frontTick() {
	if(head == 0)
		return;

	head->maxTimeToWait--;
}

Time TimerQueue::frontTime() {
	if(head == 0)
		return -1;

	return head->maxTimeToWait;
}

