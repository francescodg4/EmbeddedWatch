#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"

enum AlarmState {ALARM_OFF, ALARM_ON, ALARM_EXPIRED};
enum EWatchAlarmSignal {AL_ALARM_SET_SIG, AL_CLOCK_TICK_SIG};
enum EWatchAlarmState {AL_ALARM_OFF_STATE, AL_ALARM_ON_STATE, AL_ALARM_EXPIRED_STATE};

typedef struct {
	ClockCounter alarmCounter;
	ClockCounter finalCounter;
	ClockCounter *external;
	enum AlarmState alarmState;
	enum EWatchAlarmState state;	
} EWatchAlarm;

void EWatchAlarm_Init2(EWatchAlarm *this, ClockCounter *external);
void EWatchAlarm_Init(EWatchAlarm *this);
void EWatchAlarm_Dispatch(EWatchAlarm *this, enum EWatchAlarmSignal sig);

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm *this);
int EWatchAlarm_GetHours(EWatchAlarm *this);
int EWatchAlarm_GetMinutes(EWatchAlarm *this);

#endif /* EWATCHALARM_H */
