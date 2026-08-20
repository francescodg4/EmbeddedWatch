#include "EWatchClock.h"

void EWatchClock_Init(EWatchClock* self)
{
    ClockCounter_Init(&self->counter);
}

void EWatchClock_Dispatch(EWatchClock* self, enum EWatchClockSignal sig)
{
    if (sig == CLOCK_TICK) {
        ClockCounter_Increment(&self->counter, 1);
    }
}

int EWatchClock_GetHours(EWatchClock* self)
{
    return ClockCounter_GetHours(&self->counter);
}

int EWatchClock_GetMinutes(EWatchClock* self)
{
    return ClockCounter_GetMinutes(&self->counter);
}

int EWatchClock_GetSeconds(EWatchClock* self)
{
    return ClockCounter_GetSeconds(&self->counter);
}

int EWatchClock_GetTenths(EWatchClock* self)
{
    return ClockCounter_GetTenths(&self->counter);
}

void EWatchClock_Set(EWatchClock* self, unsigned int tenths)
{
    ClockCounter_Set(&self->counter, tenths);
}

unsigned int EWatchClock_GetCount(EWatchClock* self)
{
    return ClockCounter_GetCount(&self->counter);
}
