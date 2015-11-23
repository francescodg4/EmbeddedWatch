#ifndef EWATCH_H
#define EWATCH_H

#include "EWatchControl.h"

extern const int TENTHS_IN_DAY;
extern const int TENTHS_IN_HOUR;
extern const int TENTHS_IN_MINUTE;
extern const int TENTHS_IN_SECOND;

// enum EWatchSignal {CLOCK_TICK, SET_ALARM_MODE, SET_CLOCK_MODE};
enum EWatchSignal {CLOCK_TICK};
// enum EWatchMode {CLOCK_MODE, ALARM_MODE};
// enum EWatchState {CLOCK_STATE, ALARM_STATE};

typedef struct {
  int counter;
  EWatchControl control;
  // enum EWatchState state;
} EWatch;

void EWatch_Init(EWatch *this);
int EWatch_GetHours(EWatch *this);
int EWatch_GetMinutes(EWatch *this);
int EWatch_GetSeconds(EWatch *this);
int EWatch_GetTenths(EWatch *this);
enum EWatchMode EWatch_GetMode(EWatch *this);

void EWatch_Dispatch(EWatch *this, unsigned int sig);
// void EWatch_Dispatch(EWatch *this, EWatchSignal sig);

#endif /* EWATCH_H */
