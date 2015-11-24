#include "EWatch.h"

enum ChildFsm {CLOCK, CONTROL};

static void stopwatchOutput(EWatch *this)
{
	this->hours = EWatchStopwatch_GetHours(&this->stopwatch);
	this->minutes = EWatchStopwatch_GetMinutes(&this->stopwatch);
	this->seconds = EWatchStopwatch_GetSeconds(&this->stopwatch);
	this->tenths = EWatchStopwatch_GetTenths(&this->stopwatch);
	this->mode = STOPWATCH_MODE;
}

static void clockOutput(EWatch *this)
{
	this->hours = EWatchClock_GetHours(&this->clock);
	this->minutes = EWatchClock_GetMinutes(&this->clock);
	this->seconds = EWatchClock_GetSeconds(&this->clock);
	this->tenths = EWatchClock_GetTenths(&this->clock);
	this->mode = CLOCK_MODE;
}

/* static unsigned int convertSignal(enum EWatchSignal sig, enum ChildFsm child) */
/* { */
/* 	unsigned int offset = 0; */

/* 	if (child == CLOCK) */
/* 	  offset = EW_CLOCK_TICK_SIG; */
/* 	else if (child == CONTROL) */
/* 	  offset = EW_SET_CLOCK_MODE; */

/* 	return sig - offset; */
/* } */

static void transition(EWatch *this, enum EWatchState state)
{
	this->state = state;
}


static void clockState(EWatch *this, enum EWatchSignal sig)
{
	switch (sig) {

	case EW_CLOCK_TICK_SIG:
		clockOutput(this);
		break;

	case EW_STOPWATCH_MODE_SIG:
		transition(this, STOPWATCH_STATE);
		stopwatchOutput(this);
		break;

	default:
		break;
	}
}

void stopwatchState(EWatch *this, enum EWatchSignal sig)
{
	// Open for extension: lap view can be implemented with + and -

	switch(sig) {

	case EW_BUTTON_P_SIG:
		EWatchStopwatch_Dispatch(&this->stopwatch, ST_TOGGLE_SIG);
		break;

	case EW_BUTTON_M_SIG:
		EWatchStopwatch_Dispatch(&this->stopwatch, ST_TOGGLE_SIG);
		break;

	case EW_CLOCK_TICK_SIG:
//		EWatchStopwatch_Dispatch(&this->stopwatch, ST_CLOCK_TICK_SIG);
		stopwatchOutput(this);
		break;

	case EW_CLOCK_MODE_SIG:
		transition(this, CLOCK_STATE);
		clockOutput(this);
		break;

	default:
		break;
	}
}

void EWatch_Init(EWatch *this) 
{
	EWatchClock_Init(&this->clock);
	EWatchStopwatch_Init(&this->stopwatch);

	transition(this, CLOCK_STATE);
	clockOutput(this);

	// EWatchControl_Init(&this->control);
}

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig)
{
	/* Update concurrent time */
	if (sig == EW_CLOCK_TICK_SIG) {
		EWatchClock_Dispatch(&this->clock, CLOCK_TICK);
 		EWatchStopwatch_Dispatch(&this->stopwatch, ST_CLOCK_TICK_SIG);
	}

	switch (this->state) {

	case CLOCK_STATE:
		clockState(this, sig);		
		break;

	case STOPWATCH_STATE:
		stopwatchState(this, sig);
		break;
	
	default:
		break;
	}
	
   	/* if (sig == EW_CLOCK_TICK_SIG) */
	/* 	EWatchClock_Dispatch(&this->clock, convertSignal(sig, CLOCK)); */
	/* else if (sig >= EW_SET_CLOCK_MODE && sig <= EW_SET_ALARM_MODE) */
	/*   	EWatchControl_Dispatch(&this->control, convertSignal(sig, CONTROL));       */
}

int EWatch_GetHours(EWatch *this)
{
	return this->hours;
}

int EWatch_GetMinutes(EWatch *this)
{
	return this->minutes;
}


int EWatch_GetSeconds(EWatch *this)
{
	return this->seconds;
}

int EWatch_GetTenths(EWatch *this)
{
	return this->tenths;
}

enum EWatchMode EWatch_GetMode(EWatch *this)
{
	return this->mode;
}
