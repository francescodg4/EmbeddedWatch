#ifndef CLOCKCOUNTER_H
#define CLOCKCOUNTER_H

#ifdef __cplusplus
extern "C" {
#endif

extern const int TENTHS_IN_DAY;
extern const int TENTHS_IN_HOUR;
extern const int TENTHS_IN_MINUTE;
extern const int TENTHS_IN_SECOND;

typedef struct {
    unsigned int counter;
} ClockCounter;

void ClockCounter_Init(ClockCounter* self);

void ClockCounter_Increment(ClockCounter* self, int tenths);
void ClockCounter_Decrement(ClockCounter* self, int tenths);
void ClockCounter_Tick(ClockCounter* self);

void ClockCounter_Reset(ClockCounter* self);
void ClockCounter_Set(ClockCounter* self, unsigned int tenths);

int ClockCounter_GetHours(ClockCounter* self);
int ClockCounter_GetMinutes(ClockCounter* self);
int ClockCounter_GetSeconds(ClockCounter* self);
int ClockCounter_GetTenths(ClockCounter* self);
unsigned int ClockCounter_GetCount(ClockCounter* self);

int convertToTicks(int hours, int minutes, int seconds, int tenths);
int convertToTenths(int hours, int minutes, int seconds, int tenths);

#ifdef __cplusplus
}
#endif

#endif /* CLOCKCOUNTER_H */
