#ifndef EWATCHCLOCK_H
#define EWATCHCLOCK_H

#include "ClockCounter.h"

enum EWatchClockSignal {CLOCK_TICK};

typedef struct {
	ClockCounter counter;
} EWatchClock;

void EWatchClock_Init(EWatchClock *this);
void EWatchClock_Dispatch(EWatchClock *this, enum EWatchClockSignal sig);

void EWatchClock_Set(EWatchClock *this, unsigned int tenths);

unsigned int EWatchClock_GetCount(EWatchClock *this);
int EWatchClock_GetHours(EWatchClock *this);
int EWatchClock_GetMinutes(EWatchClock *this);
int EWatchClock_GetSeconds(EWatchClock *this);
int EWatchClock_GetTenths(EWatchClock *this);

#endif /* EWATCHCLOCK_H */
