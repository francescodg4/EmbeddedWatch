#ifndef EWATCHSTOPWATCH_H
#define EWATCHSTOPWATCH_H

#include "EWatchClock.h"

enum EWatchStopwatchSignal {ST_CLOCK_TICK_SIG, ST_TOGGLE_SIG, ST_RESET_SIG};
// enum EWatchStopwatchSignal {ST_CLOCK_TICK, ST_BUTTON_P, ST_BUTTON_M};
enum EWatchStopwatchState {ST_RUNNING_STATE, ST_STOP_STATE};

typedef struct {
	EWatchClock internal;
	enum EWatchStopwatchState state;
} EWatchStopwatch;

void EWatchStopwatch_Init(EWatchStopwatch *this);
void EWatchStopwatch_Dispatch(EWatchStopwatch *this, enum EWatchStopwatchSignal sig);
int EWatchStopwatch_GetHours(EWatchStopwatch *this);
int EWatchStopwatch_GetMinutes(EWatchStopwatch *this);
int EWatchStopwatch_GetSeconds(EWatchStopwatch *this);
int EWatchStopwatch_GetTenths(EWatchStopwatch *this);

#endif /* EWATCHSTOPWATCH_H */
