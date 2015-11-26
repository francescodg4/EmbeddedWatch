#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"

typedef struct {
	ClockCounter alarmCounter;
} EWatchAlarm;

void EWatchAlarm_Init(EWatchAlarm *this);

int EWatchAlarm_GetHours(EWatchAlarm *this);
int EWatchAlarm_GetMinutes(EWatchAlarm *this);

#endif /* EWATCHALARM_H */
