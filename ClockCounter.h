#ifndef CLOCKCOUNTER_H
#define CLOCKCOUNTER_H

extern const int TENTHS_IN_DAY;
extern const int TENTHS_IN_HOUR;
extern const int TENTHS_IN_MINUTE;
extern const int TENTHS_IN_SECOND;

typedef struct {
	unsigned int counter;
} ClockCounter;

void ClockCounter_Init(ClockCounter *this);

void ClockCounter_Increment(ClockCounter *this, unsigned int tenths);
void ClockCounter_Tick(ClockCounter *this);

void ClockCounter_Reset(ClockCounter *this);
void ClockCounter_Set(ClockCounter *this, unsigned int tenths);

int ClockCounter_GetHours(ClockCounter *this);
int ClockCounter_GetMinutes(ClockCounter *this);
int ClockCounter_GetSeconds(ClockCounter *this);
int ClockCounter_GetTenths(ClockCounter *this);
unsigned int ClockCounter_GetCount(ClockCounter *this);

#endif /* CLOCKCOUNTER_H */
