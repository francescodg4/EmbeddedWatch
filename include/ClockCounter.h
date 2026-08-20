#ifndef CLOCKCOUNTER_H
#define CLOCKCOUNTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Unit conversion constants: how many tenths of a second make up a day,
 * an hour, a minute, and a second, respectively. `ClockCounter` values are
 * always expressed in tenths of a second.
 */
extern const int TENTHS_IN_DAY;
extern const int TENTHS_IN_HOUR;
extern const int TENTHS_IN_MINUTE;
extern const int TENTHS_IN_SECOND;

/**
 * A wall-clock-style counter, in tenths of a second, that wraps around
 * every 24 hours. It is the shared primitive behind every timekeeping
 * component in this library (EWatchClock, EWatchStopwatch, EWatchTimeset).
 */
typedef struct {
    unsigned int counter;
} ClockCounter;

/** Reset self to zero. */
void ClockCounter_Init(ClockCounter* self);

/**
 * Add tenths to self, wrapping around at TENTHS_IN_DAY. Negative values
 * are ignored (no-op) - use ClockCounter_Decrement instead.
 */
void ClockCounter_Increment(ClockCounter* self, int tenths);

/**
 * Subtract tenths from self, wrapping around below zero back through
 * TENTHS_IN_DAY. Negative values are ignored (no-op) - use
 * ClockCounter_Increment instead.
 */
void ClockCounter_Decrement(ClockCounter* self, int tenths);

/** Advance self by exactly one tenth of a second, wrapping at the day boundary. */
void ClockCounter_Tick(ClockCounter* self);

/** Reset self to zero. Equivalent to ClockCounter_Init. */
void ClockCounter_Reset(ClockCounter* self);

/** Overwrite self's value directly, wrapping tenths around at TENTHS_IN_DAY. */
void ClockCounter_Set(ClockCounter* self, unsigned int tenths);

/** Return the hours component (0-23) of self's current value. */
int ClockCounter_GetHours(ClockCounter* self);

/** Return the minutes component (0-59) of self's current value. */
int ClockCounter_GetMinutes(ClockCounter* self);

/** Return the seconds component (0-59) of self's current value. */
int ClockCounter_GetSeconds(ClockCounter* self);

/** Return the tenths-of-a-second component (0-9) of self's current value. */
int ClockCounter_GetTenths(ClockCounter* self);

/** Return self's raw value, in tenths of a second since the last wrap. */
unsigned int ClockCounter_GetCount(ClockCounter* self);

/**
 * Combine an hours/minutes/seconds/tenths breakdown into a single tenths-
 * of-a-second count, suitable for ClockCounter_Set. Does not itself wrap
 * the result at TENTHS_IN_DAY.
 */
int convertToTenths(int hours, int minutes, int seconds, int tenths);

#ifdef __cplusplus
}
#endif

#endif /* CLOCKCOUNTER_H */
