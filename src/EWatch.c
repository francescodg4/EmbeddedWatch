#include "EWatch.h"
#include "EWatchTimeset.h"

static EWatchTimeset timeset;
static void updateOutput(EWatch* self, enum EWatchMode mode);

static void transition(EWatch* self, enum EWatchState state);
static void clockState(EWatch* self, enum EWatchSignal sig);
static void stopwatchState(EWatch* self, enum EWatchSignal sig);
static void timesetState(EWatch* self, enum EWatchSignal sig);
static void alarmState(EWatch* self, enum EWatchSignal sig);

void EWatch_Init(EWatch* self)
{
    EWatchClock_Init(&self->clock);
    EWatchStopwatch_Init(&self->stopwatch);
    EWatchTimeset_Init(&timeset);
    EWatchAlarm_Init(&self->alarm, &self->clock.counter);

    self->state = clockState;

    transition(self, CLOCK_STATE);
    updateOutput(self, CLOCK_MODE);
}

void EWatch_Dispatch(EWatch* self, enum EWatchSignal sig)
{
    /* Update concurrent time */
    if (sig == EW_CLOCK_TICK_SIG) {
        EWatchClock_Dispatch(&self->clock, CLOCK_TICK);
        EWatchStopwatch_Dispatch(&self->stopwatch, ST_CLOCK_TICK_SIG);
        EWatchAlarm_Dispatch(&self->alarm, AL_CLOCK_TICK_SIG);
    }

    (*self->state)(self, sig);
}

int EWatch_GetHours(EWatch* self)
{
    return self->hours;
}

int EWatch_GetMinutes(EWatch* self)
{
    return self->minutes;
}

int EWatch_GetSeconds(EWatch* self)
{
    return self->seconds;
}

int EWatch_GetTenths(EWatch* self)
{
    return self->tenths;
}

enum EWatchMode EWatch_GetMode(EWatch* self)
{
    return self->mode;
}

enum AlarmState EWatch_GetAlarmState(EWatch* self)
{
    return EWatchAlarm_GetAlarmState(&self->alarm);
}

// --------------- Private functions --------------- //

static void transition(EWatch* self, enum EWatchState state)
{
    (*self->state)(self, EXIT_SIG);

    /* State update */
    switch (state) {

    case CLOCK_STATE:
        self->state = clockState;
        break;

    case STOPWATCH_STATE:
        self->state = stopwatchState;
        break;

    case TIMESET_STATE:
        self->state = timesetState;
        break;

    case ALARM_STATE:
        self->state = alarmState;
        break;

    default:
        break;
    }

    (*self->state)(self, ENTRY_SIG);
}

static void timesetState(EWatch* self, enum EWatchSignal sig)
{
    int hours = 0;
    int minutes = 0;
    int time = 0;

    switch (sig) {
    case ENTRY_SIG:
        time = EWatchClock_GetCount(&self->clock);
        EWatchTimeset_Set(&timeset, time);
        break;

    case EXIT_SIG:
        hours = EWatchTimeset_GetHours(&timeset);
        minutes = EWatchTimeset_GetMinutes(&timeset);
        EWatchClock_Set(&self->clock, convertToTenths(hours, minutes, 0, 0));
        break;

    case EW_BUTTON_P_SIG:
        EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
        updateOutput(self, TIMESET_MODE);
        break;

    case EW_BUTTON_M_SIG:
        EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);
        updateOutput(self, TIMESET_MODE);
        break;

    case EW_TIMESET_MODE_SIG:
        EWatchTimeset_Dispatch(&timeset, TS_TOGGLE_MODE_SIG);
        updateOutput(self, TIMESET_MODE);
        break;

    case EW_CLOCK_MODE_SIG:
        transition(self, CLOCK_STATE);
        updateOutput(self, CLOCK_MODE);
        break;

    case EW_STOPWATCH_MODE_SIG:
        transition(self, STOPWATCH_STATE);
        updateOutput(self, STOPWATCH_MODE);
        break;

    case EW_ALARM_MODE_SIG:
        transition(self, ALARM_STATE);
        updateOutput(self, ALARM_MODE);
        break;

    default:
        break;
    }
}

static void clockState(EWatch* self, enum EWatchSignal sig)
{
    switch (sig) {

    case EW_CLOCK_TICK_SIG:
        updateOutput(self, CLOCK_MODE);
        break;

    case EW_TIMESET_MODE_SIG:
        transition(self, TIMESET_STATE);
        updateOutput(self, TIMESET_STATE);
        break;

    case EW_STOPWATCH_MODE_SIG:
        transition(self, STOPWATCH_STATE);
        updateOutput(self, STOPWATCH_MODE);
        break;

    case EW_ALARM_MODE_SIG:
        transition(self, ALARM_STATE);
        updateOutput(self, ALARM_MODE);
        break;

    default:
        break;
    }
}

static void stopwatchState(EWatch* self, enum EWatchSignal sig)
{
    // Open for extension: lap view can be implemented with + and -

    switch (sig) {

    case EW_BUTTON_P_SIG:
        EWatchStopwatch_Dispatch(&self->stopwatch, ST_TOGGLE_SIG);
        break;

    case EW_BUTTON_M_SIG:
        EWatchStopwatch_Dispatch(&self->stopwatch, ST_RESET_SIG);
        updateOutput(self, STOPWATCH_MODE);
        break;

    case EW_CLOCK_TICK_SIG:
        updateOutput(self, STOPWATCH_MODE);
        break;

    case EW_TIMESET_MODE_SIG:
        transition(self, TIMESET_STATE);
        updateOutput(self, TIMESET_MODE);
        break;

    case EW_CLOCK_MODE_SIG:
        transition(self, CLOCK_STATE);
        updateOutput(self, CLOCK_MODE);
        break;

    case EW_ALARM_MODE_SIG:
        transition(self, ALARM_STATE);
        updateOutput(self, ALARM_MODE);
        break;

    default:
        break;
    }
}

static void alarmState(EWatch* self, enum EWatchSignal sig)
{
    switch (sig) {
    case EW_ALARM_MODE_SIG:
        EWatchAlarm_Dispatch(&self->alarm, AL_ALARM_SET_SIG);
        updateOutput(self, ALARM_MODE);
        break;

    case EW_BUTTON_P_SIG:
        EWatchAlarm_Dispatch(&self->alarm, AL_INC_SIG);
        updateOutput(self, ALARM_MODE);
        break;

    case EW_BUTTON_M_SIG:
        EWatchAlarm_Dispatch(&self->alarm, AL_DEC_SIG);
        updateOutput(self, ALARM_MODE);
        break;

    case EW_STOPWATCH_MODE_SIG:
        transition(self, STOPWATCH_STATE);
        updateOutput(self, STOPWATCH_MODE);
        break;

    case EW_CLOCK_MODE_SIG:
        transition(self, CLOCK_STATE);
        updateOutput(self, CLOCK_MODE);
        break;

    case EW_TIMESET_MODE_SIG:
        transition(self, TIMESET_STATE);
        updateOutput(self, TIMESET_MODE);
        break;

    default:
        break;
    }
}

static void updateOutput(EWatch* self, enum EWatchMode mode)
{
    switch (mode) {
    case TIMESET_MODE:
        self->hours = EWatchTimeset_GetHours(&timeset);
        self->minutes = EWatchTimeset_GetMinutes(&timeset);
        self->seconds = 0;
        self->tenths = 0;
        break;

    case CLOCK_MODE:
        self->hours = EWatchClock_GetHours(&self->clock);
        self->minutes = EWatchClock_GetMinutes(&self->clock);
        self->seconds = EWatchClock_GetSeconds(&self->clock);
        self->tenths = EWatchClock_GetTenths(&self->clock);
        break;

    case STOPWATCH_MODE:
        self->hours = EWatchStopwatch_GetHours(&self->stopwatch);
        self->minutes = EWatchStopwatch_GetMinutes(&self->stopwatch);
        self->seconds = EWatchStopwatch_GetSeconds(&self->stopwatch);
        self->tenths = EWatchStopwatch_GetTenths(&self->stopwatch);
        break;

    case ALARM_MODE:
        self->hours = EWatchAlarm_GetHours(&self->alarm);
        self->minutes = EWatchAlarm_GetMinutes(&self->alarm);
        self->seconds = 0;
        self->tenths = 0;
        break;

    default:
        break;
    }

    self->alarmState = EWatchAlarm_GetAlarmState(&self->alarm);
    self->mode = mode;
}
