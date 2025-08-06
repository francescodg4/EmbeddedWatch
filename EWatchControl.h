#ifndef EWATCHCONTROL_H
#define EWATHCCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

enum EWatchControlSignal {SET_CLOCK_MODE, SET_ALARM_MODE};
enum EWatchMode {CLOCK_MODE, ALARM_MODE};
enum EWatchControlState {CLOCK_STATE, ALARM_STATE};

typedef struct {
  enum EWatchMode mode;
  enum EWatchControlState state;
} EWatchControl;

void EWatchControl_Init(EWatchControl *this);
void EWatchControl_Transition(EWatchControl *this, enum EWatchControlState state);
void EWatchControl_Dispatch(EWatchControl *this, enum EWatchControlSignal sig);
enum EWatchMode EWatchControl_GetMode(EWatchControl *this);

#ifdef __cplusplus
}
#endif

#endif /* EWATCHCONTROL_H */
