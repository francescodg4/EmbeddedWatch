#ifndef EWATCHTIMESET_H
#define EWATCHTIMESET_H

#include "EWatchClock.h"
#include "ClockCounter.h"

enum EWatchTimesetSignal {
	TS_INC_HOURS_SIG,
	TS_DEC_HOURS_SIG,
	TS_INC_MINUTES_SIG,
	TS_DEC_MINUTES_SIG,
	TS_SET_HOURS_MODE_SIG,
	TS_SET_MINUTES_MODE_SIG
};

enum EWatchTimesetState {TS_SET_HOURS_STATE, TS_SET_MINUTES_STATE};

typedef struct {
	ClockCounter internal;
	enum EWatchTimesetState state;
} EWatchTimeset;

void EWatchTimeset_Init(EWatchTimeset *this);
void EWatchTimeset_Dispatch(EWatchTimeset *this, enum EWatchTimesetSignal sig);
int EWatchTimeset_GetHours(EWatchTimeset *this);
int EWatchTimeset_GetMinutes(EWatchTimeset *this);
int EWatchTimeset_GetSeconds(EWatchTimeset *this);
int EWatchTimeset_GetTenths(EWatchTimeset *this);

#endif /* EWATCHTIMESET_H */
