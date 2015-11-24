#include "EWatchControl.h"

void EWatchControl_Init(EWatchControl *this)
{
	this->state = CLOCK_STATE;
	this->mode = CLOCK_MODE;
}

void EWatchControl_Transition(EWatchControl *this, enum EWatchControlState state)
{
	this->state = state;
}

void EWatchControl_Dispatch(EWatchControl *this, enum EWatchControlSignal sig)
{
	switch(this->state) {
	case CLOCK_STATE:
		switch(sig) {
		case SET_ALARM_MODE:
			EWatchControl_Transition(this, ALARM_STATE);
			this->mode = ALARM_MODE;
			break;
		case SET_CLOCK_MODE:
			EWatchControl_Transition(this, CLOCK_STATE);
			this->mode = CLOCK_MODE;
			break;
		default:
			break;
		}
		break;

	case ALARM_STATE:
		switch(sig) {
		case SET_ALARM_MODE:
			break;
		case SET_CLOCK_MODE:
			EWatchControl_Transition(this, CLOCK_STATE);
			this->mode = CLOCK_MODE;
			break;
		default:
			break;
		}

		break;
	}
}

enum EWatchMode EWatchControl_GetMode(EWatchControl *this)
{
	return this->mode;
}
