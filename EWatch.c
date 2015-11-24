#include "EWatch.h"

void EWatch_Init(EWatch *this) 
{
	EWatchClock_Init(&this->clock);
	EWatchControl_Init(&this->control);
}

void EWatch_Dispatch(EWatch *this, const unsigned int sig[2])
{
	EWatchClock_Dispatch(&this->clock, sig[0]);
	EWatchControl_Dispatch(&this->control, sig[1]);
}
int EWatch_GetHours(EWatch *this)
{
  	return EWatchClock_GetHours(&this->clock);
}

int EWatch_GetMinutes(EWatch *this)
{
	return EWatchClock_GetMinutes(&this->clock);
}


int EWatch_GetSeconds(EWatch *this)
{
	return EWatchClock_GetSeconds(&this->clock);
}

int EWatch_GetTenths(EWatch *this)
{
	return EWatchClock_GetTenths(&this->clock);
}

enum EWatchMode EWatch_GetMode(EWatch *this)
{
	return EWatchControl_GetMode(&this->control);
}
