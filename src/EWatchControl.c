#include "EWatchControl.h"

void EWatchControl_Init(EWatchControl* self)
{
    self->state = CONTROL_CLOCK_STATE;
    self->mode = CLOCK_MODE;
}

void EWatchControl_Transition(EWatchControl* self, enum EWatchControlState state)
{
    self->state = state;
}

void EWatchControl_Dispatch(EWatchControl* self, enum EWatchControlSignal sig)
{
    switch (self->state) {
    case CONTROL_CLOCK_STATE:
        switch (sig) {
        case SET_ALARM_MODE:
            EWatchControl_Transition(self, CONTROL_ALARM_STATE);
            self->mode = ALARM_MODE;
            break;
        case SET_CLOCK_MODE:
            EWatchControl_Transition(self, CONTROL_CLOCK_STATE);
            self->mode = CLOCK_MODE;
            break;
        default:
            break;
        }
        break;

    case CONTROL_ALARM_STATE:
        switch (sig) {
        case SET_ALARM_MODE:
            break;
        case SET_CLOCK_MODE:
            EWatchControl_Transition(self, CONTROL_CLOCK_STATE);
            self->mode = CLOCK_MODE;
            break;
        default:
            break;
        }

        break;
    }
}

enum EWatchMode EWatchControl_GetMode(EWatchControl* self)
{
    return self->mode;
}
