#include "EWatchClock.h"

/* const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;
*/

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
