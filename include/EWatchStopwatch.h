#ifndef EWATCHSTOPWATCH_H
#define EWATCHSTOPWATCH_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

enum EWatchStopwatchSignal {
    ST_CLOCK_TICK_SIG,
    ST_TOGGLE_SIG,
    ST_RESET_SIG
};

enum EWatchStopwatchState {
    ST_RUNNING_STATE,
    ST_STOP_STATE
};

typedef struct {
    ClockCounter internal;
    enum EWatchStopwatchState state;
} EWatchStopwatch;

void EWatchStopwatch_Init(EWatchStopwatch* self);

void EWatchStopwatch_Dispatch(EWatchStopwatch* self, enum EWatchStopwatchSignal sig);

int EWatchStopwatch_GetHours(EWatchStopwatch* self);

int EWatchStopwatch_GetMinutes(EWatchStopwatch* self);

int EWatchStopwatch_GetSeconds(EWatchStopwatch* self);

int EWatchStopwatch_GetTenths(EWatchStopwatch* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHSTOPWATCH_H */
