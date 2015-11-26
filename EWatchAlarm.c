#include "EWatchAlarm.h"

void EWatchAlarm_Init(EWatchAlarm *this)
{
	unsigned int defaultTime = convertToTenths(12, 0, 0, 0);
	ClockCounter_Init(&this->alarmCounter);
	ClockCounter_Set(&this->alarmCounter, defaultTime);
}

int EWatchAlarm_GetHours(EWatchAlarm *this)
{
	return ClockCounter_GetHours(&this->alarmCounter);
}

int EWatchAlarm_GetMinutes(EWatchAlarm *this)
{
	return ClockCounter_GetMinutes(&this->alarmCounter);
}
