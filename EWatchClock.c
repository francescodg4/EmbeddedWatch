#include "EWatchClock.h"

void EWatchClock_Init(EWatchClock *this)
{
	ClockCounter_Init(&this->counter);
}

void EWatchClock_Dispatch(EWatchClock *this, enum EWatchClockSignal sig)
{
   	if (sig == CLOCK_TICK)
		ClockCounter_Increment(&this->counter, 1);
}

int EWatchClock_GetHours(EWatchClock *this)
{
	return ClockCounter_GetHours(&this->counter);
}

int EWatchClock_GetMinutes(EWatchClock *this)
{
	return ClockCounter_GetMinutes(&this->counter);
}


int EWatchClock_GetSeconds(EWatchClock *this)
{
	return ClockCounter_GetSeconds(&this->counter);
}

int EWatchClock_GetTenths(EWatchClock *this)
{
	return ClockCounter_GetTenths(&this->counter);
}

void EWatchClock_Set(EWatchClock *this, unsigned int tenths)
{
	ClockCounter_Set(&this->counter, tenths);
}

unsigned int EWatchClock_GetCount(EWatchClock *this)
{
	return ClockCounter_GetCount(&this->counter);
}
