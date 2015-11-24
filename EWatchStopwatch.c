#include "EWatchStopwatch.h"

static void transition(EWatchStopwatch *this, enum EWatchStopwatchState state)
{
	this->state = state;
}

static void reset(EWatchStopwatch *this)
{
	this->internal.counter = 0;
}

void EWatchStopwatch_Init(EWatchStopwatch *this)
{
	EWatchClock_Init(&this->internal);
	transition(this, ST_STOP_STATE);
	reset(this);	
}

void EWatchStopwatch_Dispatch(EWatchStopwatch *this, enum EWatchStopwatchSignal sig)
{
	switch (this->state) {
	case ST_RUNNING_STATE:
		switch (sig) {

		case ST_CLOCK_TICK_SIG:
			EWatchClock_Dispatch(&this->internal, CLOCK_TICK);
			break;
		
		case ST_TOGGLE_SIG:
			transition(this, ST_STOP_STATE);
			break;

		default:
			break;
		}

		break;

	case ST_STOP_STATE:
		switch (sig) {

		case ST_TOGGLE_SIG:
			transition(this, ST_RUNNING_STATE);
			break;

		case ST_RESET_SIG:
			reset(this);
		default:
			break;
		}

		break;
	}
}

int EWatchStopwatch_GetHours(EWatchStopwatch *this)
{
	return EWatchClock_GetHours(&this->internal);
}

int EWatchStopwatch_GetMinutes(EWatchStopwatch *this)
{
	return EWatchClock_GetMinutes(&this->internal);
}

int EWatchStopwatch_GetSeconds(EWatchStopwatch *this)
{
	return EWatchClock_GetSeconds(&this->internal);
}

int EWatchStopwatch_GetTenths(EWatchStopwatch *this)
{
	return EWatchClock_GetTenths(&this->internal);
}
