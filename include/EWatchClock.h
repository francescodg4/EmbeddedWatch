#ifndef EWATCHCLOCK_H
#define EWATCHCLOCK_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Signals understood by EWatchClock_Dispatch. */
enum EWatchClockSignal {
    CLOCK_TICK /**< Advance the clock by one tenth of a second. */
};

/** The watch's running time-of-day clock. */
typedef struct {
    ClockCounter counter;
} EWatchClock;

/** Reset self to 00:00:00.0. */
void EWatchClock_Init(EWatchClock* self);

/** Dispatch sig to self; CLOCK_TICK advances the clock by one tenth of a second. */
void EWatchClock_Dispatch(EWatchClock* self, enum EWatchClockSignal sig);

/** Overwrite self's current time, in tenths of a second since midnight. */
void EWatchClock_Set(EWatchClock* self, unsigned int tenths);

/** Return self's current time, in tenths of a second since midnight. */
unsigned int EWatchClock_GetCount(EWatchClock* self);

/** Return the hours component (0-23) of self's current time. */
int EWatchClock_GetHours(EWatchClock* self);

/** Return the minutes component (0-59) of self's current time. */
int EWatchClock_GetMinutes(EWatchClock* self);

/** Return the seconds component (0-59) of self's current time. */
int EWatchClock_GetSeconds(EWatchClock* self);

/** Return the tenths-of-a-second component (0-9) of self's current time. */
int EWatchClock_GetTenths(EWatchClock* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHCLOCK_H */
