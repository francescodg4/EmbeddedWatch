#include "EWatchAlarm.h"
#include "EWatchTimeset.h"

static void transition(EWatchAlarm* self, EWatchAlarmState state);

static void alarmOffState(EWatchAlarm* self, enum EWatchAlarmSignal sig);
static void alarmOnState(EWatchAlarm* self, enum EWatchAlarmSignal sig);
static void alarmExpiredState(EWatchAlarm* self, enum EWatchAlarmSignal sig);

static void setHoursState(EWatchAlarm* self, enum EWatchAlarmSignal sig);
static void setMinutesState(EWatchAlarm* self, enum EWatchAlarmSignal sig);

void EWatchAlarm_Init(EWatchAlarm* self, ClockCounter* external)
{
    unsigned int defaultTime = convertToTenths(12, 0, 0, 0);

    EWatchTimeset_Init(&self->expirationTime);
    EWatchTimeset_Set(&self->expirationTime, defaultTime);

    self->external = external;
    self->alarmState = ALARM_OFF;

    transition(self, alarmOffState);
}

void EWatchAlarm_Dispatch(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    (*self->state)(self, sig);
}

int EWatchAlarm_GetHours(EWatchAlarm* self)
{
    return EWatchTimeset_GetHours(&self->expirationTime);
}

int EWatchAlarm_GetMinutes(EWatchAlarm* self)
{
    return EWatchTimeset_GetMinutes(&self->expirationTime);
}

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm* self)
{
    return self->alarmState;
}

static void transition(EWatchAlarm* self, EWatchAlarmState state)
{
    self->state = state;
}

static void alarmOffState(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    switch (sig) {
    case AL_ALARM_SET_SIG:
        transition(self, setHoursState);

        // prevExpirationTime = ClockCounter_GetCount(&self->expirationTime);
        // EWatchTimeset_Init(&timeset);
        // EWatchTimeset_Set(&self->expirationTime, prevExpirationTime);

        break;

    default:
        break;
    }
}

static void alarmOnState(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    unsigned int prevExpirationTime;

    switch (sig) {
    case AL_CLOCK_TICK_SIG:
        if (ClockCounter_GetCount(self->external) == EWatchTimeset_GetCount(&self->expirationTime)) {
            transition(self, alarmExpiredState);
            self->alarmState = ALARM_EXPIRED;
        }
        break;

    case AL_ALARM_SET_SIG:
        transition(self, setHoursState);

        prevExpirationTime = EWatchTimeset_GetCount(&self->expirationTime);
        EWatchTimeset_Init(&self->expirationTime);
        EWatchTimeset_Set(&self->expirationTime, prevExpirationTime);

        self->alarmState = ALARM_OFF;
        break;

    default:
        break;
    }
}

static void alarmExpiredState(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    switch (sig) {
    case AL_ALARM_SET_SIG:
        self->alarmState = ALARM_OFF;
        transition(self, alarmOffState);
        break;
    default:
        break;
    }
}

static void setHoursState(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    switch (sig) {

    case AL_INC_SIG:
        EWatchTimeset_Dispatch(&self->expirationTime, TS_INC_SIG);
        break;

    case AL_DEC_SIG:
        EWatchTimeset_Dispatch(&self->expirationTime, TS_DEC_SIG);
        break;

    case AL_ALARM_SET_SIG:
        EWatchTimeset_Dispatch(&self->expirationTime, TS_TOGGLE_MODE_SIG);
        transition(self, setMinutesState);
        break;

    default:
        break;
    }
}

static void setMinutesState(EWatchAlarm* self, enum EWatchAlarmSignal sig)
{
    switch (sig) {

    case AL_INC_SIG:
        EWatchTimeset_Dispatch(&self->expirationTime, TS_INC_SIG);
        break;

    case AL_DEC_SIG:
        EWatchTimeset_Dispatch(&self->expirationTime, TS_DEC_SIG);
        break;

    case AL_ALARM_SET_SIG:
        transition(self, alarmOnState);
        self->alarmState = ALARM_ON;
        break;

    default:
        break;
    }
}
