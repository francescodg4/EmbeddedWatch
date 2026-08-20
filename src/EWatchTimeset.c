#include "EWatchTimeset.h"

static void transition(EWatchTimeset* self, enum EWatchTimesetState state)
{
    self->state = state;
}

void EWatchTimeset_Init(EWatchTimeset* self)
{
    transition(self, TS_SET_HOURS_STATE);
    ClockCounter_Init(&self->internal);
}

void EWatchTimeset_Dispatch(EWatchTimeset* self, enum EWatchTimesetSignal sig)
{
    switch (self->state) {
    case TS_SET_HOURS_STATE:
        switch (sig) {
        case TS_TOGGLE_MODE_SIG:
            transition(self, TS_SET_MINUTES_STATE);
            break;

        case TS_INC_SIG:
            ClockCounter_Increment(&self->internal, TENTHS_IN_HOUR);
            break;

        case TS_DEC_SIG:
            ClockCounter_Decrement(&self->internal, TENTHS_IN_HOUR);
            break;

        case TS_SET_MINUTES_MODE_SIG:
            transition(self, TS_SET_MINUTES_STATE);
            break;

        default:
            break;
        }

        break;

    case TS_SET_MINUTES_STATE:
        switch (sig) {
        case TS_TOGGLE_MODE_SIG:
            transition(self, TS_SET_HOURS_STATE);
            break;

        case TS_INC_SIG:
            ClockCounter_Increment(&self->internal, TENTHS_IN_MINUTE);
            break;

        case TS_DEC_SIG:
            ClockCounter_Decrement(&self->internal, TENTHS_IN_MINUTE);
            break;

        case TS_SET_HOURS_MODE_SIG:
            transition(self, TS_SET_HOURS_STATE);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void EWatchTimeset_Set(EWatchTimeset* self, unsigned int tenths)
{
    ClockCounter_Set(&self->internal, tenths);
}

int EWatchTimeset_GetHours(EWatchTimeset* self)
{
    return ClockCounter_GetHours(&self->internal);
}

int EWatchTimeset_GetMinutes(EWatchTimeset* self)
{
    return ClockCounter_GetMinutes(&self->internal);
}

int EWatchTimeset_GetSeconds(EWatchTimeset* self)
{
    return ClockCounter_GetSeconds(&self->internal);
}

int EWatchTimeset_GetTenths(EWatchTimeset* self)
{
    return ClockCounter_GetTenths(&self->internal);
}

unsigned int EWatchTimeset_GetCount(EWatchTimeset* self)
{
    return ClockCounter_GetCount(&self->internal);
}
