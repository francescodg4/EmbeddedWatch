#include "EWatch.h"

const int TENTHS_IN_DAY = 864000;
const int TENTHS_IN_HOUR = 36000;
const int TENTHS_IN_MINUTE = 600;
const int TENTHS_IN_SECOND = 10;

static void Clock_Init(EWatch *this)
{
	this->counter = 0;
}

static void Clock_Dispatch(EWatch *this, enum EWatchSignal sig)
{
	if (sig == CLOCK_TICK)
		this->counter = (this->counter + 1) % TENTHS_IN_DAY;
}

/* static void Control_Init(EWatch *this) */
/* { */
/* 	this->state = CLOCK_STATE; */
/* 	this->mode = CLOCK_MODE; */
/* } */

/* static void Control_Transition(EWatch *this, enum EWatchState state) */
/* { */
/* 	this->state = state; */
/* } */

/* static void Control_Dispatch(EWatch *this, enum EWatchSignal sig) */
/* { */
/* 	switch(this->state) { */
/* 	case CLOCK_STATE: */
/* 		switch(sig) { */
/* 		case SET_ALARM_MODE: */
/* 			Control_Transition(this, ALARM_STATE); */
/* 			this->mode = ALARM_MODE; */
/* 			break; */
/* 		case SET_CLOCK_MODE: */
/* 			Control_Transition(this, CLOCK_STATE); */
/* 			this->mode = CLOCK_MODE; */
/* 			break; */
/* 		default: */
/* 			break; */
/* 		} */
/* 		break; */

/* 	case ALARM_STATE: */
/* 		switch(sig) { */
/* 		case SET_ALARM_MODE: */
/* 			break; */
/* 		case SET_CLOCK_MODE: */
/* 			Control_Transition(this, CLOCK_STATE); */
/* 			this->mode = CLOCK_MODE; */
/* 			break; */
/* 		default: */
/* 			break; */
/* 		} */

/* 		break; */
/* 	} */
/* } */

void EWatch_Init(EWatch *this) 
{
	Clock_Init(this);
	EWatchControl_Init(&this->control);
}

void EWatch_Dispatch(EWatch *this, const unsigned int sig[2])
{
	Clock_Dispatch(this, sig[0]);
	EWatchControl_Dispatch(&this->control, sig[1]);
}

int EWatch_GetHours(EWatch *this)
{
	return (this->counter / TENTHS_IN_HOUR) % 24;
}

int EWatch_GetMinutes(EWatch *this)
{
	return (this->counter / TENTHS_IN_MINUTE) % 60;
}


int EWatch_GetSeconds(EWatch *this)
{
	return (this->counter / TENTHS_IN_SECOND) % 60;
}

int EWatch_GetTenths(EWatch *this)
{
	return this->counter % 10;
}

enum EWatchMode EWatch_GetMode(EWatch *this)
{
	return EWatchControl_GetMode(&this->control);
}
