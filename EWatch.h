#ifndef EWATCH_H
#define EWATCH_H

// #include "EWatchControl.h"
#include "EWatchClock.h"
#include "EWatchStopwatch.h"

enum EWatchSignal {
	EW_CLOCK_TICK_SIG, 
	EW_STOPWATCH_MODE_SIG,
	EW_CLOCK_MODE_SIG,
	EW_BUTTON_P_SIG,
	EW_BUTTON_M_SIG,
	EW_TIMESET_MODE_SIG,
	EW_SET_ALARM_MODE,
	EXIT_SIG,
	ENTRY_SIG
};

// enum EWatchSignal {CLOCK_TICK};
// enum EWatchControlSignal {SET_ALARM_MODE = 3, SET_CLOCK_MODE};
enum EWatchMode {CLOCK_MODE, STOPWATCH_MODE, ALARM_MODE, TIMESET_MODE};
enum EWatchState {CLOCK_STATE, ALARM_STATE, STOPWATCH_STATE, TIMESET_STATE};
 
typedef struct EWatch_ EWatch;

struct EWatch_ {
	int hours;
	int minutes;
	int seconds;
	int tenths;
	enum EWatchMode mode;

  // int counter;
	// EWatchControl control;
	EWatchClock clock;
	EWatchStopwatch stopwatch;
	void (*state)(EWatch *, enum EWatchSignal);
	// State state;
};

void EWatch_Init(EWatch *this);
int EWatch_GetHours(EWatch *this);
int EWatch_GetMinutes(EWatch *this);
int EWatch_GetSeconds(EWatch *this);
int EWatch_GetTenths(EWatch *this);
enum EWatchMode EWatch_GetMode(EWatch *this);

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig);
// void EWatch_Dispatch(EWatch *this, unsigned int sig);
// void EWatch_Dispatch(EWatch *this, EWatchSignal sig);

#endif /* EWATCH_H */
