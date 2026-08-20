#include "EWatchStopwatch.h"

static void transition(EWatchStopwatch* self, enum EWatchStopwatchState state)
{
    self->state = state;
}

static void reset(EWatchStopwatch* self)
{
    ClockCounter_Reset(&self->internal);
}

void EWatchStopwatch_Init(EWatchStopwatch* self)
{
    ClockCounter_Init(&self->internal);
    transition(self, ST_STOP_STATE);
}

void EWatchStopwatch_Dispatch(EWatchStopwatch* self, enum EWatchStopwatchSignal sig)
{
    switch (self->state) {
    case ST_RUNNING_STATE:
        switch (sig) {

        case ST_CLOCK_TICK_SIG:
            ClockCounter_Tick(&self->internal);
            break;

        case ST_TOGGLE_SIG:
            transition(self, ST_STOP_STATE);
            break;

        default:
            break;
        }

        break;

    case ST_STOP_STATE:
        switch (sig) {

        case ST_TOGGLE_SIG:
            transition(self, ST_RUNNING_STATE);
            break;

        case ST_RESET_SIG:
            reset(self);
            break;

        default:
            break;
        }

        break;
    }
}

int EWatchStopwatch_GetHours(EWatchStopwatch* self)
{
    return ClockCounter_GetHours(&self->internal);
}

int EWatchStopwatch_GetMinutes(EWatchStopwatch* self)
{
    return ClockCounter_GetMinutes(&self->internal);
}

int EWatchStopwatch_GetSeconds(EWatchStopwatch* self)
{
    return ClockCounter_GetSeconds(&self->internal);
}

int EWatchStopwatch_GetTenths(EWatchStopwatch* self)
{
    return ClockCounter_GetTenths(&self->internal);
}
