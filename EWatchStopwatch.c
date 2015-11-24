#include "EWatchStopwatch.h"

void EWatchStopwatch_Init(EWatchStopwatch *this)
{
	EWatchClock_Init(&this->internal);
}
void EWatchStopwatch_Dispatch(EWatchStopwatch *this, enum EWatchStopwatchSignal sig)
{
	EWatchClock_Dispatch(&this->internal, sig);
}

int EWatchStopwatch_GetHours(EWatchStopwatch *this)
{
	return EWatchClock_GetHours(&this->internal);
}

int EWatchStopwatch_GetMinutes(EWatchStopwatch *this)
{
	return EWatchClock_GetMinutes(&this->internal);
}

int EWatchStopwatch_GetSeconds(EWatchStopwatch *this)
{
	return EWatchClock_GetSeconds(&this->internal);
}

int EWatchStopwatch_GetTenths(EWatchStopwatch *this)
{
	return EWatchClock_GetTenths(&this->internal);
}
