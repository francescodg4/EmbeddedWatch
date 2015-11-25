#include "EWatchTimeset.h"

void EWatchTimeset_Init(EWatchTimeset *this)
{
	EWatchClock_Init(&this->internal);
}

void EWatchTimeset_Dispatch(EWatchTimeset *this, enum EWatchTimesetSignal sig)
{
	
}

int EWatchTimeset_GetHours(EWatchTimeset *this)
{
	return 0;
}

int EWatchTimeset_GetMinutes(EWatchTimeset *this)
{
	return 0;
}

int EWatchTimeset_GetSeconds(EWatchTimeset *this)
{
	return 0;
}

int EWatchTimeset_GetTenths(EWatchTimeset *this)
{
	return 0;
}

