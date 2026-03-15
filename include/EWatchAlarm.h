#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"
#include "EWatchTimeset.h"

#ifdef __cplusplus
extern "C" {
#endif

enum AlarmState {
    ALARM_OFF,
    ALARM_ON,
    ALARM_EXPIRED
};

enum AlarmDisplayState {
    ALARM_DISPLAY_NONE,
    ALARM_DISPLAY_SET_HOURS,
    ALARM_DISPLAY_SET_MINUTES,
};

enum EWatchAlarmSignal {
    AL_ALARM_SET_SIG,
    AL_CLOCK_TICK_SIG,
    AL_INC_SIG,
    AL_DEC_SIG
};

typedef struct EWatchAlarm_ EWatchAlarm;
typedef void (*EWatchAlarmState)(EWatchAlarm*, enum EWatchAlarmSignal);

struct EWatchAlarm_ {
    EWatchTimeset expirationTime;
    ClockCounter* external;
    enum AlarmState alarmState;
    EWatchAlarmState state;
    enum AlarmDisplayState displayState;
};

void EWatchAlarm_Init(EWatchAlarm* self, ClockCounter* external);

void EWatchAlarm_Dispatch(EWatchAlarm* self, enum EWatchAlarmSignal sig);

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm* self);

enum AlarmDisplayState EWatchAlarm_GetAlarmDisplayState(EWatchAlarm* self);

int EWatchAlarm_GetHours(EWatchAlarm* self);

int EWatchAlarm_GetMinutes(EWatchAlarm* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHALARM_H */
