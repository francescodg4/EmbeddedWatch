#ifndef EWATCH_H
#define EWATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "EWatchEnums.h"
#include "EWatchControl.h"
#include "EWatchClock.h"
#include "EWatchStopwatch.h"
#include "EWatchAlarm.h"

/** Signals dispatched to the top-level EWatch state machine. */
enum EWatchSignal {
    EW_CLOCK_TICK_SIG, /**< Periodic tick (e.g. every 100ms); advances the clock, stopwatch, and alarm. */
    EW_STOPWATCH_MODE_SIG, /**< Switch to stopwatch mode, or (while already there) toggle start/stop. */
    EW_CLOCK_MODE_SIG, /**< Switch to clock mode. */
    EW_BUTTON_P_SIG, /**< The mode-dependent "+" button. */
    EW_BUTTON_M_SIG, /**< The mode-dependent "-" button. */
    EW_TIMESET_MODE_SIG, /**< Switch to timeset mode, or (while already there) toggle hours/minutes editing. */
    EW_ALARM_MODE_SIG, /**< Switch to alarm mode, or (while already there) advance alarm setup. */
    EXIT_SIG, /**< Internal: sent to a state on exit, during a mode transition. */
    ENTRY_SIG /**< Internal: sent to a state on entry, during a mode transition. */
};

/** Which top-level mode the watch is currently displaying. */
enum EWatchState {
    CLOCK_STATE,
    ALARM_STATE,
    STOPWATCH_STATE,
    TIMESET_STATE
};

typedef struct EWatch_ EWatch;

/**
 * The top-level digital watch state machine. Combines an EWatchClock,
 * EWatchStopwatch, and EWatchAlarm behind a single mode-switching
 * interface: exactly one of clock/stopwatch/timeset/alarm mode is
 * displayed at a time (see EWatch_GetMode), while the clock, stopwatch,
 * and alarm all keep advancing in the background regardless of which
 * mode is currently shown.
 */
struct EWatch_ {
    /** Currently displayed time/alarm-state, refreshed on each relevant EWatch_Dispatch call. */
    int hours;
    int minutes;
    int seconds;
    int tenths;

    enum AlarmState alarmState;
    enum EWatchMode mode;

    EWatchControl control; /**< Currently unused; reserved for future mode-control state. */
    EWatchClock clock;
    EWatchStopwatch stopwatch;
    EWatchAlarm alarm;

    void (*state)(EWatch*, enum EWatchSignal);
};

/** Initialize self in clock mode, with the clock, stopwatch, and alarm all at their defaults. */
void EWatch_Init(EWatch* self);

/** Return the hours component (0-23) of self's currently displayed mode. */
int EWatch_GetHours(EWatch* self);

/** Return the minutes component (0-59) of self's currently displayed mode. */
int EWatch_GetMinutes(EWatch* self);

/** Return the seconds component (0-59) of self's currently displayed mode. */
int EWatch_GetSeconds(EWatch* self);

/** Return the tenths-of-a-second component (0-9) of self's currently displayed mode. */
int EWatch_GetTenths(EWatch* self);

/** Return which mode self is currently displaying. */
enum EWatchMode EWatch_GetMode(EWatch* self);

/** Return self's current alarm status, regardless of which mode is displayed. */
enum AlarmState EWatch_GetAlarmState(EWatch* self);

/**
 * Dispatch sig to self, the sole entry point for driving the watch. On
 * every call, EW_CLOCK_TICK_SIG first advances the clock, stopwatch, and
 * alarm by one tick; sig is then routed to whichever mode is currently
 * active, which may switch modes and/or refresh the displayed fields.
 */
void EWatch_Dispatch(EWatch* self, enum EWatchSignal sig);

#ifdef __cplusplus
}
#endif

#endif /* EWATCH_H */
