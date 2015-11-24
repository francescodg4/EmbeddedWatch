#include "EWatch.h"

enum ChildFsm {CLOCK, CONTROL};

static unsigned int convertSignal(enum EWatchSignal sig, enum ChildFsm child)
{
	unsigned int offset = 0;

	if (child == CLOCK)
	  offset = EW_CLOCK_TICK;
	else if (child == CONTROL)
	  offset = EW_SET_CLOCK_MODE;

	return sig - offset;
}

void EWatch_Init(EWatch *this) 
{
	EWatchClock_Init(&this->clock);
	EWatchControl_Init(&this->control);
}

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig)
{
   	if (sig == EW_CLOCK_TICK)
		EWatchClock_Dispatch(&this->clock, convertSignal(sig, CLOCK));
	else if (sig >= EW_SET_CLOCK_MODE && sig <= EW_SET_ALARM_MODE)
	  	EWatchControl_Dispatch(&this->control, convertSignal(sig, CONTROL));      
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
