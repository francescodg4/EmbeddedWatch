#ifndef EWATCHSTOPWATCH_H
#define EWATCHSTOPWATCH_H

#include "ClockCounter.h"

enum EWatchStopwatchSignal {ST_CLOCK_TICK_SIG, ST_TOGGLE_SIG, ST_RESET_SIG};
enum EWatchStopwatchState {ST_RUNNING_STATE, ST_STOP_STATE};

typedef struct {
	ClockCounter internal;
	enum EWatchStopwatchState state;
} EWatchStopwatch;

void EWatchStopwatch_Init(EWatchStopwatch *this);
void EWatchStopwatch_Dispatch(EWatchStopwatch *this, enum EWatchStopwatchSignal sig);

int EWatchStopwatch_GetHours(EWatchStopwatch *this);
int EWatchStopwatch_GetMinutes(EWatchStopwatch *this);
int EWatchStopwatch_GetSeconds(EWatchStopwatch *this);
int EWatchStopwatch_GetTenths(EWatchStopwatch *this);

#endif /* EWATCHSTOPWATCH_H */
