#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"

enum AlarmState {ALARM_OFF, ALARM_ON, ALARM_EXPIRED};

typedef struct {
	ClockCounter alarmCounter;
	enum AlarmState alarmState;
} EWatchAlarm;

void EWatchAlarm_Init(EWatchAlarm *this);

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm *this);
int EWatchAlarm_GetHours(EWatchAlarm *this);
int EWatchAlarm_GetMinutes(EWatchAlarm *this);

#endif /* EWATCHALARM_H */
