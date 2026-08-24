#ifndef EWATCHSTOPWATCH_H
#define EWATCHSTOPWATCH_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Signals understood by EWatchStopwatch_Dispatch. */
enum EWatchStopwatchSignal {
    ST_CLOCK_TICK_SIG, /**< Advance the elapsed time by one tenth of a second, only while running. */
    ST_TOGGLE_SIG, /**< Start the stopwatch if stopped, or stop it if running. */
    ST_RESET_SIG /**< Reset the elapsed time to zero; ignored while running. */
};

/** Internal run state of an EWatchStopwatch. */
enum EWatchStopwatchState {
    ST_RUNNING_STATE,
    ST_STOP_STATE
};

/** A start/stop/reset stopwatch, independent of the watch's time-of-day clock. */
typedef struct {
    ClockCounter internal;
    enum EWatchStopwatchState state;
} EWatchStopwatch;

/** Reset self to zero, stopped. */
void EWatchStopwatch_Init(EWatchStopwatch* self);

/** Dispatch sig to self; see EWatchStopwatchSignal for behavior. */
void EWatchStopwatch_Dispatch(EWatchStopwatch* self, enum EWatchStopwatchSignal sig);

/** Return the hours component (0-23) of self's elapsed time. */
int EWatchStopwatch_GetHours(EWatchStopwatch* self);

/** Return the minutes component (0-59) of self's elapsed time. */
int EWatchStopwatch_GetMinutes(EWatchStopwatch* self);

/** Return the seconds component (0-59) of self's elapsed time. */
int EWatchStopwatch_GetSeconds(EWatchStopwatch* self);

/** Return the tenths-of-a-second component (0-9) of self's elapsed time. */
int EWatchStopwatch_GetTenths(EWatchStopwatch* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHSTOPWATCH_H */
