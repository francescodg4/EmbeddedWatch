#include "EWatchStopwatch.h"

static void transition(EWatchStopwatch *this, enum EWatchStopwatchState state)
{
	this->state = state;
}

static void reset(EWatchStopwatch *this)
{
	ClockCounter_Reset(&this->internal);
}

void EWatchStopwatch_Init(EWatchStopwatch *this)
{
	ClockCounter_Init(&this->internal);
	transition(this, ST_STOP_STATE);
}

void EWatchStopwatch_Dispatch(EWatchStopwatch *this, enum EWatchStopwatchSignal sig)
{
	switch (this->state) {
	case ST_RUNNING_STATE:
		switch (sig) {

		case ST_CLOCK_TICK_SIG:			
			ClockCounter_Tick(&this->internal);
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
			break;

		default:
			break;
		}

		break;
	}
}

int EWatchStopwatch_GetHours(EWatchStopwatch *this)
{
	return ClockCounter_GetHours(&this->internal);
}

int EWatchStopwatch_GetMinutes(EWatchStopwatch *this)
{
	return ClockCounter_GetMinutes(&this->internal);
}

int EWatchStopwatch_GetSeconds(EWatchStopwatch *this)
{
	return ClockCounter_GetSeconds(&this->internal);
}

int EWatchStopwatch_GetTenths(EWatchStopwatch *this)
{
	return ClockCounter_GetTenths(&this->internal);
}
