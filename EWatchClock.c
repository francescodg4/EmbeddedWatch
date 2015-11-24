#include "EWatchClock.h"

const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;

void EWatchClock_Init(EWatchClock *this)
{
	this->counter = 0;
}

void EWatchClock_Dispatch(EWatchClock *this, enum EWatchClockSignal sig)
{
	if (sig == CLOCK_TICK)
		this->counter = (this->counter + 1) % TENTHS_IN_DAY;
}

int EWatchClock_GetHours(EWatchClock *this)
{
	return (this->counter / TENTHS_IN_HOUR) % 24;
}

int EWatchClock_GetMinutes(EWatchClock *this)
{
	return (this->counter / TENTHS_IN_MINUTE) % 60;
}


int EWatchClock_GetSeconds(EWatchClock *this)
{
	return (this->counter / TENTHS_IN_SECOND) % 60;
}

int EWatchClock_GetTenths(EWatchClock *this)
{
	return this->counter % 10;
}
