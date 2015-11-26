#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"
#include "EWatchTimeset.h"

enum AlarmState {ALARM_OFF, ALARM_ON, ALARM_EXPIRED};
enum EWatchAlarmSignal {AL_ALARM_SET_SIG, AL_CLOCK_TICK_SIG, AL_INC_SIG, AL_DEC_SIG};

enum EWatchAlarmState {
	AL_ALARM_OFF_STATE,
	AL_ALARM_ON_STATE,
	AL_ALARM_EXPIRED_STATE,
	AL_SET_ALARM_HOURS_STATE,
	AL_SET_ALARM_MINUTES_STATE
};

typedef struct {
	ClockCounter alarmCounter;
	EWatchTimeset expirationTime;
	ClockCounter *external;
	enum AlarmState alarmState;
	enum EWatchAlarmState state;	
} EWatchAlarm;

void EWatchAlarm_Init(EWatchAlarm *this, ClockCounter *external);
void EWatchAlarm_Dispatch(EWatchAlarm *this, enum EWatchAlarmSignal sig);

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm *this);
int EWatchAlarm_GetHours(EWatchAlarm *this);
int EWatchAlarm_GetMinutes(EWatchAlarm *this);

#endif /* EWATCHALARM_H */
