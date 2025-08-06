#ifndef EWATCHCONTROL_H
#define EWATCHCONTROL_H

#include "EWatchEnums.h"

#ifdef __cplusplus
extern "C" {
#endif

enum EWatchControlSignal {
    SET_CLOCK_MODE,
    SET_ALARM_MODE
};

enum EWatchControlState {
    CONTROL_CLOCK_STATE,
    CONTROL_ALARM_STATE
};

typedef struct {
    enum EWatchMode mode;
    enum EWatchControlState state;
} EWatchControl;

void EWatchControl_Init(EWatchControl* self);
void EWatchControl_Transition(EWatchControl* self, enum EWatchControlState state);
void EWatchControl_Dispatch(EWatchControl* self, enum EWatchControlSignal sig);
enum EWatchMode EWatchControl_GetMode(EWatchControl* self);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHCONTROL_H */
