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

enum EWatchSignal {
    EW_CLOCK_TICK_SIG,
    EW_STOPWATCH_MODE_SIG,
    EW_CLOCK_MODE_SIG,
    EW_BUTTON_P_SIG,
    EW_BUTTON_M_SIG,
    EW_TIMESET_MODE_SIG,
    EW_ALARM_MODE_SIG,
    EXIT_SIG,
    ENTRY_SIG
};

enum EWatchState {
    CLOCK_STATE,
    ALARM_STATE,
    STOPWATCH_STATE,
    TIMESET_STATE
};

typedef struct EWatch_ EWatch;

struct EWatch_ {
    int hours;
    int minutes;
    int seconds;
    int tenths;
    enum AlarmState alarmState;
    enum EWatchMode mode;

    EWatchControl control;
    EWatchClock clock;
    EWatchStopwatch stopwatch;
    EWatchAlarm alarm;
    void (*state)(EWatch*, enum EWatchSignal);
};

void EWatch_Init(EWatch* self);
int EWatch_GetHours(EWatch* self);
int EWatch_GetMinutes(EWatch* self);
int EWatch_GetSeconds(EWatch* self);
int EWatch_GetTenths(EWatch* self);
enum EWatchMode EWatch_GetMode(EWatch* self);
enum AlarmState EWatch_GetAlarmState(EWatch* self);
void EWatch_Dispatch(EWatch* self, enum EWatchSignal sig);

#ifdef __cplusplus
}
#endif

#endif /* EWATCH_H */
