#ifndef EWATCHTIMESET_H
#define EWATCHTIMESET_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

enum EWatchTimesetSignal {
	TS_INC_SIG,
	TS_DEC_SIG,
	TS_SET_HOURS_MODE_SIG,
	TS_SET_MINUTES_MODE_SIG,
	TS_TOGGLE_MODE_SIG
};

enum EWatchTimesetState {TS_SET_HOURS_STATE, TS_SET_MINUTES_STATE};

typedef struct {
	ClockCounter internal;
	enum EWatchTimesetState state;
} EWatchTimeset;

void EWatchTimeset_Init(EWatchTimeset *self);
void EWatchTimeset_Dispatch(EWatchTimeset *self, enum EWatchTimesetSignal sig);

void EWatchTimeset_Set(EWatchTimeset *self, unsigned int tenths); 

unsigned int EWatchTimeset_GetCount(EWatchTimeset *self);
int EWatchTimeset_GetHours(EWatchTimeset *self);
int EWatchTimeset_GetMinutes(EWatchTimeset *self);
int EWatchTimeset_GetSeconds(EWatchTimeset *self);
int EWatchTimeset_GetTenths(EWatchTimeset *self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHTIMESET_H */
