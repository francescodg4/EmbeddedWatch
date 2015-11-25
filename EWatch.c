#include "EWatch.h"
#include "EWatchTimeset.h"

// enum ChildFsm {CLOCK, CONTROL};

static EWatchTimeset timeset;
static void updateOutput(EWatch *this, enum EWatchMode mode);

// static void stopwatchOutput(EWatch *this);
// static void clockOutput(EWatch *this);

static void transition(EWatch *this, enum EWatchState state);
static void clockState(EWatch *this, enum EWatchSignal sig);
static void stopwatchState(EWatch *this, enum EWatchSignal sig);
static void timesetState(EWatch *this, enum EWatchSignal sig);

void EWatch_Init(EWatch *this) 
{
	EWatchClock_Init(&this->clock);
	EWatchStopwatch_Init(&this->stopwatch);
	EWatchTimeset_Init(&timeset);

	transition(this, CLOCK_STATE);
	updateOutput(this, CLOCK_MODE);

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

	case TIMESET_STATE:
		timesetState(this, sig);
		break;

	default:
		break;
	}
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

// --------------- Private functions --------------- //

static void transition(EWatch *this, enum EWatchState state)
{
	this->state = state;
}

static void timesetState(EWatch *this, enum EWatchSignal sig)
{
	switch (sig) {
	case EW_BUTTON_P_SIG:
		EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
		updateOutput(this, TIMESET_MODE);
		break;

	case EW_BUTTON_M_SIG:
		EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);
		updateOutput(this, TIMESET_MODE);
		break;

	case EW_TIMESET_MODE_SIG:
		EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);
		updateOutput(this, TIMESET_MODE);
		break;

	case EW_CLOCK_MODE_SIG:
		transition(this, CLOCK_STATE);

		/* Update clock */
		int hours = EWatchTimeset_GetHours(&timeset);
		int minutes = EWatchTimeset_GetMinutes(&timeset);		
		EWatchClock_Set(&this->clock, convertToTenths(hours, minutes, 0, 0));

		updateOutput(this, CLOCK_MODE);
	default:
		break;
	}	
}


static void clockState(EWatch *this, enum EWatchSignal sig)
{
	switch (sig) {

	case EW_CLOCK_TICK_SIG:
		updateOutput(this, CLOCK_MODE);
		break;

	case EW_TIMESET_MODE_SIG:
		transition(this, TIMESET_STATE);
		// Entry action
		int time = EWatchClock_GetCount(&this->clock);
		EWatchTimeset_Set(&timeset, time);
		updateOutput(this, TIMESET_STATE);
		break;

	case EW_STOPWATCH_MODE_SIG:
		transition(this, STOPWATCH_STATE);
		updateOutput(this, STOPWATCH_MODE);
		break;

	default:
		break;
	}
}

static void stopwatchState(EWatch *this, enum EWatchSignal sig)
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
		updateOutput(this, STOPWATCH_MODE);
		break;

	case EW_CLOCK_MODE_SIG:
		transition(this, CLOCK_STATE);
		updateOutput(this, CLOCK_MODE);
		break;

	default:
		break;
	}
}

static void updateOutput(EWatch *this, enum EWatchMode mode)
{
	switch (mode) {
	case TIMESET_MODE:
		this->hours = EWatchTimeset_GetHours(&timeset);
		this->minutes = EWatchTimeset_GetMinutes(&timeset);
		this->seconds = 0;
		this->tenths = 0;
		break;

	case CLOCK_MODE:
		this->hours = EWatchClock_GetHours(&this->clock);
		this->minutes = EWatchClock_GetMinutes(&this->clock);
		this->seconds = EWatchClock_GetSeconds(&this->clock);
		this->tenths = EWatchClock_GetTenths(&this->clock);
		break;

	case STOPWATCH_MODE:
		this->hours = EWatchStopwatch_GetHours(&this->stopwatch);
		this->minutes = EWatchStopwatch_GetMinutes(&this->stopwatch);
		this->seconds = EWatchStopwatch_GetSeconds(&this->stopwatch);
		this->tenths = EWatchStopwatch_GetTenths(&this->stopwatch);
		break;

	default:
		break;
	}

	this->mode = mode;
}

/* static void stopwatchOutput(EWatch *this) */
/* { */
/* 	this->hours = EWatchStopwatch_GetHours(&this->stopwatch); */
/* 	this->minutes = EWatchStopwatch_GetMinutes(&this->stopwatch); */
/* 	this->seconds = EWatchStopwatch_GetSeconds(&this->stopwatch); */
/* 	this->tenths = EWatchStopwatch_GetTenths(&this->stopwatch); */
/* 	this->mode = STOPWATCH_MODE; */
/* } */

/* static void clockOutput(EWatch *this) */
/* { */
/* 	this->hours = EWatchClock_GetHours(&this->clock); */
/* 	this->minutes = EWatchClock_GetMinutes(&this->clock); */
/* 	this->seconds = EWatchClock_GetSeconds(&this->clock); */
/* 	this->tenths = EWatchClock_GetTenths(&this->clock); */
/* 	this->mode = CLOCK_MODE; */
/* } */
