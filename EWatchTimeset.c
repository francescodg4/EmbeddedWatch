#include "EWatchTimeset.h"

static void transition(EWatchTimeset* this, enum EWatchTimesetState state)
{
    this->state = state;
}

void EWatchTimeset_Init(EWatchTimeset* this)
{
    transition(this, TS_SET_HOURS_STATE);
    ClockCounter_Init(&this->internal);
}

void EWatchTimeset_Dispatch(EWatchTimeset* this, enum EWatchTimesetSignal sig)
{
    switch (this->state) {
    case TS_SET_HOURS_STATE:
        switch (sig) {
        case TS_TOGGLE_MODE_SIG:
            transition(this, TS_SET_MINUTES_STATE);
            break;

        case TS_INC_SIG:
            ClockCounter_Increment(&this->internal, TENTHS_IN_HOUR);
            break;

        case TS_DEC_SIG:
            ClockCounter_Decrement(&this->internal, TENTHS_IN_HOUR);
            break;

        case TS_SET_MINUTES_MODE_SIG:
            transition(this, TS_SET_MINUTES_STATE);
            break;

        default:
            break;
        }

        break;

    case TS_SET_MINUTES_STATE:
        switch (sig) {
        case TS_TOGGLE_MODE_SIG:
            transition(this, TS_SET_HOURS_STATE);
            break;

        case TS_INC_SIG:
            ClockCounter_Increment(&this->internal, TENTHS_IN_MINUTE);
            break;

        case TS_DEC_SIG:
            ClockCounter_Decrement(&this->internal, TENTHS_IN_MINUTE);
            break;

        case TS_SET_HOURS_MODE_SIG:
            transition(this, TS_SET_HOURS_STATE);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void EWatchTimeset_Set(EWatchTimeset* this, unsigned int tenths)
{
    ClockCounter_Set(&this->internal, tenths);
}

int EWatchTimeset_GetHours(EWatchTimeset* this)
{
    return ClockCounter_GetHours(&this->internal);
}

int EWatchTimeset_GetMinutes(EWatchTimeset* this)
{
    return ClockCounter_GetMinutes(&this->internal);
}

int EWatchTimeset_GetSeconds(EWatchTimeset* this)
{
    return ClockCounter_GetSeconds(&this->internal);
}

int EWatchTimeset_GetTenths(EWatchTimeset* this)
{
    return ClockCounter_GetTenths(&this->internal);
}

unsigned int EWatchTimeset_GetCount(EWatchTimeset* this)
{
    return ClockCounter_GetCount(&this->internal);
}
