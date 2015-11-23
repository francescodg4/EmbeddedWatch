#ifndef EWATCH_H
#define EWATCH_H

#define TENTHS_IN_DAY 864000

enum EWatchSignal {CLOCK_TICK};

typedef struct {
  int counter;
} EWatch;

void EWatch_Init(EWatch *this);
int EWatch_GetHours(EWatch *this);
int EWatch_GetMinutes(EWatch *this);
int EWatch_GetSeconds(EWatch *this);
int EWatch_GetTenths(EWatch *this);
void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig);

#endif /* EWATCH_H */
