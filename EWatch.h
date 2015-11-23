#ifndef EWATCH_H
#define EWATCH_H

extern const int TENTHS_IN_DAY;
extern const int TENTHS_IN_HOUR;
extern const int TENTHS_IN_MINUTE;
extern const int TENTHS_IN_SECOND;

enum EWatchSignal {CLOCK_TICK, SET_ALARM_MODE, SET_CLOCK_MODE};
enum EWatchMode {CLOCK_MODE, ALARM_MODE};
enum EWatchState {CLOCK_STATE, ALARM_STATE};

typedef struct {
  int counter;
  enum EWatchMode mode;
  enum EWatchState state;
} EWatch;

void EWatch_Init(EWatch *this);
int EWatch_GetHours(EWatch *this);
int EWatch_GetMinutes(EWatch *this);
int EWatch_GetSeconds(EWatch *this);
int EWatch_GetTenths(EWatch *this);
enum EWatchMode EWatch_GetMode(EWatch *this);

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig);

#endif /* EWATCH_H */
