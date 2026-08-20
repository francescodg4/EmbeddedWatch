#ifndef EWATCHALARM_H
#define EWATCHALARM_H

#include "ClockCounter.h"
#include "EWatchTimeset.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Current status of an EWatchAlarm. */
enum AlarmState {
    ALARM_OFF, /**< No alarm is armed. */
    ALARM_ON, /**< An alarm is armed and waiting to expire. */
    ALARM_EXPIRED /**< The armed alarm's expiration time has been reached. */
};

/** Signals understood by EWatchAlarm_Dispatch. */
enum EWatchAlarmSignal {
    AL_ALARM_SET_SIG, /**< Advance the off -> set-hours -> set-minutes -> on sequence by one step
                            (or, from ALARM_EXPIRED, turn the alarm off). */
    AL_CLOCK_TICK_SIG, /**< Check whether the external clock has reached the expiration time. */
    AL_INC_SIG, /**< Increment the field currently being edited (only while setting hours/minutes). */
    AL_DEC_SIG /**< Decrement the field currently being edited (only while setting hours/minutes). */
};

typedef struct EWatchAlarm_ EWatchAlarm;
typedef void (*EWatchAlarmState)(EWatchAlarm*, enum EWatchAlarmSignal);

/** An alarm that expires when an external clock reaches its configured time. */
struct EWatchAlarm_ {
    EWatchTimeset expirationTime;
    ClockCounter* external;
    enum AlarmState alarmState;
    EWatchAlarmState state;
};

/**
 * Initialize self with a default expiration time of 12:00, alarm off.
 * external is the ClockCounter this alarm compares against on every
 * AL_CLOCK_TICK_SIG (typically the watch's own EWatchClock counter); self
 * does not take ownership of it and external must outlive self.
 */
void EWatchAlarm_Init(EWatchAlarm* self, ClockCounter* external);

/** Dispatch sig to self; see EWatchAlarmSignal for behavior. */
void EWatchAlarm_Dispatch(EWatchAlarm* self, enum EWatchAlarmSignal sig);

/** Return self's current AlarmState. */
enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm* self);

/** Return the hours component (0-23) of self's configured expiration time. */
int EWatchAlarm_GetHours(EWatchAlarm* self);

/** Return the minutes component (0-59) of self's configured expiration time. */
int EWatchAlarm_GetMinutes(EWatchAlarm* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHALARM_H */
