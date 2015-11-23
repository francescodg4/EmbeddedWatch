#include "EWatch.h"

const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;

void EWatch_Init(EWatch *this) 
{
	this->counter = 0;
}

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig)
{
	this->counter = (this->counter + 1) % TENTHS_IN_DAY;
}

int EWatch_GetHours(EWatch *this)
{
	return (this->counter / TENTHS_IN_HOUR) % 24;
}

int EWatch_GetMinutes(EWatch *this)
{
	return (this->counter / TENTHS_IN_MINUTE) % 60;
}


int EWatch_GetSeconds(EWatch *this)
{
	return (this->counter / TENTHS_IN_SECOND) % 60;
}

int EWatch_GetTenths(EWatch *this)
{
	return this->counter % 10;
}
