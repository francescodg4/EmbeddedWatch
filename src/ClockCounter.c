#include "ClockCounter.h"

const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;

void ClockCounter_Init(ClockCounter* self)
{
    ClockCounter_Reset(self);
}

void ClockCounter_Increment(ClockCounter* self, int tenths)
{
    if (tenths < 0) {
        return;
    }
    self->counter = (self->counter + tenths) % TENTHS_IN_DAY;
}

void ClockCounter_Decrement(ClockCounter* self, int tenths)
{
    if (tenths < 0) {
        return;
    }
    self->counter = (TENTHS_IN_DAY + self->counter - tenths) % TENTHS_IN_DAY;
}

void ClockCounter_Tick(ClockCounter* self)
{
    self->counter = (self->counter + 1) % TENTHS_IN_DAY;
}

void ClockCounter_Reset(ClockCounter* self)
{
    self->counter = 0;
}

void ClockCounter_Set(ClockCounter* self, unsigned int tenths)
{
    self->counter = (tenths) % TENTHS_IN_DAY;
}

unsigned int ClockCounter_GetCount(ClockCounter* self)
{
    return self->counter;
}

int ClockCounter_GetHours(ClockCounter* self)
{
    return (self->counter / TENTHS_IN_HOUR) % 24;
}

int ClockCounter_GetMinutes(ClockCounter* self)
{
    return (self->counter / TENTHS_IN_MINUTE) % 60;
}

int ClockCounter_GetSeconds(ClockCounter* self)
{
    return (self->counter / TENTHS_IN_SECOND) % 60;
}

int ClockCounter_GetTenths(ClockCounter* self)
{
    return self->counter % 10;
}

// int convertToTicks(int hours, int minutes, int seconds, int tenths)
// {
//     return convertToTenths(hours, minutes, seconds, tenths);
// }

int convertToTenths(int hours, int minutes, int seconds, int tenths)
{
    return (hours * TENTHS_IN_HOUR) + (minutes * TENTHS_IN_MINUTE) + (seconds * TENTHS_IN_SECOND) + tenths;
}
