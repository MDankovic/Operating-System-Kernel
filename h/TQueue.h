#ifndef _TQUEUE_H_
#define _TQUEUE_H_

#include "Queue.h"

class TimerQueue : public Queue
{
public:
	virtual void put(PCB *_pcb, Time _maxTimeToWait = 0);

	void frontTick();
	Time frontTime();
	void addFrontTime(Time _timeToAdd);

	// dobro bi bilo da redefinisem pop_front da automatski odrzava strukturu da ne moram rucno da vodim racuna o tome

protected:

private:

};

#endif /* TQUEUE_H_ */
