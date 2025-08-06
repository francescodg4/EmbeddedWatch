#ifndef EWATCHCLOCK_H
#define EWATCHCLOCK_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

enum EWatchClockSignal { CLOCK_TICK };

typedef struct {
    ClockCounter counter;
} EWatchClock;

void EWatchClock_Init(EWatchClock* self);
void EWatchClock_Dispatch(EWatchClock* self, enum EWatchClockSignal sig);

void EWatchClock_Set(EWatchClock* self, unsigned int tenths);

unsigned int EWatchClock_GetCount(EWatchClock* self);
int EWatchClock_GetHours(EWatchClock* self);
int EWatchClock_GetMinutes(EWatchClock* self);
int EWatchClock_GetSeconds(EWatchClock* self);
int EWatchClock_GetTenths(EWatchClock* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHCLOCK_H */
