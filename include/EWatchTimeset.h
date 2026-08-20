#ifndef EWATCHTIMESET_H
#define EWATCHTIMESET_H

#include "ClockCounter.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Signals understood by EWatchTimeset_Dispatch. */
enum EWatchTimesetSignal {
    TS_INC_SIG, /**< Increment the field currently being edited (hours or minutes). */
    TS_DEC_SIG, /**< Decrement the field currently being edited (hours or minutes). */
    TS_SET_HOURS_MODE_SIG, /**< Switch to editing hours. */
    TS_SET_MINUTES_MODE_SIG, /**< Switch to editing minutes. */
    TS_TOGGLE_MODE_SIG /**< Toggle between editing hours and editing minutes. */
};

/** Which field TS_INC_SIG/TS_DEC_SIG currently affect. */
enum EWatchTimesetState {
    TS_SET_HOURS_STATE,
    TS_SET_MINUTES_STATE
};

/**
 * A generic hours:minutes editor built on a ClockCounter. Used both for
 * setting the watch's clock and for setting an alarm's expiration time.
 */
typedef struct {
    ClockCounter internal;
    enum EWatchTimesetState state;
} EWatchTimeset;

/** Reset self to zero, editing hours. */
void EWatchTimeset_Init(EWatchTimeset* self);

/** Dispatch sig to self; see EWatchTimesetSignal for behavior. */
void EWatchTimeset_Dispatch(EWatchTimeset* self, enum EWatchTimesetSignal sig);

/** Overwrite the value being edited, in tenths of a second since midnight. */
void EWatchTimeset_Set(EWatchTimeset* self, unsigned int tenths);

/** Return the value being edited, in tenths of a second since midnight. */
unsigned int EWatchTimeset_GetCount(EWatchTimeset* self);

/** Return the hours component (0-23) of the value being edited. */
int EWatchTimeset_GetHours(EWatchTimeset* self);

/** Return the minutes component (0-59) of the value being edited. */
int EWatchTimeset_GetMinutes(EWatchTimeset* self);

/** Return the seconds component (0-59) of the value being edited. */
int EWatchTimeset_GetSeconds(EWatchTimeset* self);

/** Return the tenths-of-a-second component (0-9) of the value being edited. */
int EWatchTimeset_GetTenths(EWatchTimeset* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHTIMESET_H */
