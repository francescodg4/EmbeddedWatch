#include "ClockCounter.h"

const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;

void ClockCounter_Init(ClockCounter* this)
{
    ClockCounter_Reset(this);
}

void ClockCounter_Increment(ClockCounter* this, int tenths)
{
    if (tenths < 0) {
        return;
    }
    this->counter = (this->counter + tenths) % TENTHS_IN_DAY;
}

void ClockCounter_Decrement(ClockCounter* this, int tenths)
{
    if (tenths < 0) {
        return;
    }
    this->counter = (TENTHS_IN_DAY + this->counter - tenths) % TENTHS_IN_DAY;
}

void ClockCounter_Tick(ClockCounter* this)
{
    this->counter = (this->counter + 1) % TENTHS_IN_DAY;
}

void ClockCounter_Reset(ClockCounter* this)
{
    this->counter = 0;
}

void ClockCounter_Set(ClockCounter* this, unsigned int tenths)
{
    this->counter = (tenths) % TENTHS_IN_DAY;
}

unsigned int ClockCounter_GetCount(ClockCounter* this)
{
    return this->counter;
}

int ClockCounter_GetHours(ClockCounter* this)
{
    return (this->counter / TENTHS_IN_HOUR) % 24;
}

int ClockCounter_GetMinutes(ClockCounter* this)
{
    return (this->counter / TENTHS_IN_MINUTE) % 60;
}

int ClockCounter_GetSeconds(ClockCounter* this)
{
    return (this->counter / TENTHS_IN_SECOND) % 60;
}

int ClockCounter_GetTenths(ClockCounter* this)
{
    return this->counter % 10;
}

int convertToTicks(int hours, int minutes, int seconds, int tenths)
{
    return convertToTenths(hours, minutes, seconds, tenths);
}

int convertToTenths(int hours, int minutes, int seconds, int tenths)
{
    int ticks = 0;

    ticks += hours * TENTHS_IN_HOUR;
    ticks += minutes * TENTHS_IN_MINUTE;
    ticks += seconds * TENTHS_IN_SECOND;
    ticks += tenths;

    return ticks;
}
