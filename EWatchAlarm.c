#include "EWatchAlarm.h"
#include "EWatchTimeset.h"

// static EWatchTimeset timeset;
// static void transition(EWatchAlarm *this, enum EWatchAlarmState state);
static void transition(EWatchAlarm *this, EWatchAlarmState state);

static void alarmOffState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmOnState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmExpiredState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmSetHoursState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmSetMinutesState(EWatchAlarm *this, enum EWatchAlarmSignal sig);

void EWatchAlarm_Init(EWatchAlarm *this, ClockCounter *external)
{
	unsigned int defaultTime = convertToTenths(12, 0, 0, 0);
	// ClockCounter_Set(&this->expirationTime, defaultTime);
	EWatchTimeset_Init(&this->expirationTime);
	EWatchTimeset_Set(&this->expirationTime, defaultTime);

	this->external = external;	
	this->alarmState = ALARM_OFF;

	transition(this, alarmOffState);
}

void EWatchAlarm_Dispatch(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	// unsigned int prevExpirationTime;

	(*this->state)(this, sig);

	/* switch (this->state) { */
	/* case AL_ALARM_OFF_STATE: */
	/* 	switch (sig) { */
	/* 	case AL_ALARM_SET_SIG: */
	/* 		transition(this, AL_SET_ALARM_HOURS_STATE); */

	/* 		// prevExpirationTime = ClockCounter_GetCount(&this->expirationTime); */
	/* 		// EWatchTimeset_Init(&timeset); */
	/* 		// EWatchTimeset_Set(&this->expirationTime, prevExpirationTime); */

	/* 		break; */

	/* 	default: */
	/* 		break; */
	/* 	} */

	/* 	break; */

	/* case AL_SET_ALARM_HOURS_STATE: */
	/* 	switch (sig) { */

	/* 	case AL_INC_SIG: */
	/* 		EWatchTimeset_Dispatch(&this->expirationTime, TS_INC_SIG); */
	/* 		break; */

	/* 	case AL_DEC_SIG: */
	/* 		EWatchTimeset_Dispatch(&this->expirationTime, TS_DEC_SIG); */
	/* 		break; */

	/* 	case AL_ALARM_SET_SIG: */
	/* 		EWatchTimeset_Dispatch(&this->expirationTime, TS_TOGGLE_MODE_SIG); */
	/* 		transition(this, AL_SET_ALARM_MINUTES_STATE); */
	/* 		break; */

	/* 	default: */
	/* 		break; */
	/* 	} */

	/* 	break; */

	/* case AL_SET_ALARM_MINUTES_STATE: */
	/* 	switch (sig) { */

	/* 	case AL_INC_SIG: */
	/* 		EWatchTimeset_Dispatch(&this->expirationTime, TS_INC_SIG); */
	/* 		break; */

	/* 	case AL_DEC_SIG: */
	/* 		EWatchTimeset_Dispatch(&this->expirationTime, TS_DEC_SIG); */
	/* 		break; */

	/* 	case AL_ALARM_SET_SIG: */
	/* 		transition(this, AL_ALARM_ON_STATE); */
	/* 		this->alarmState = ALARM_ON; */
	/* 		break; */

	/* 	default: */
	/* 		break; */
	/* 	}		 */

	/* 	break; */

	/* case AL_ALARM_ON_STATE: */
	/* 	switch (sig) { */
	/* 	case AL_CLOCK_TICK_SIG: */
	/* 		/\* */
	/* 		if (ClockCounter_GetCount(this->external) ==  */
	/* 		    ClockCounter_GetCount(&this->expirationTime)) { */
	/* 			transition(this, AL_ALARM_EXPIRED_STATE); */
	/* 			this->alarmState = ALARM_EXPIRED; */
	/* 		} */
	/* 		*\/ */
	/* 		break; */
		
	/* 	default: */
	/* 		break; */
	/* 	} */

	/* 	break; */

	/* case AL_ALARM_EXPIRED_STATE: */
	/* 	break; */

	/* default: */
	/* 	break; */
	/* } */
}

int EWatchAlarm_GetHours(EWatchAlarm *this)
{
	return EWatchTimeset_GetHours(&this->expirationTime);
}

int EWatchAlarm_GetMinutes(EWatchAlarm *this)
{
	return EWatchTimeset_GetMinutes(&this->expirationTime);
}

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm *this)
{
	return this->alarmState;
}

static void transition(EWatchAlarm *this, EWatchAlarmState state)
{
	this->state = state;
}

static void alarmOffState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (sig) {
	case AL_ALARM_SET_SIG:
		transition(this, alarmSetHoursState);

		// prevExpirationTime = ClockCounter_GetCount(&this->expirationTime);
		// EWatchTimeset_Init(&timeset);
		// EWatchTimeset_Set(&this->expirationTime, prevExpirationTime);

		break;

	default:
		break;
	}	
}

static void alarmOnState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (sig) {
	case AL_CLOCK_TICK_SIG:
		/*
		  if (ClockCounter_GetCount(this->external) ==
		  ClockCounter_GetCount(&this->expirationTime)) {
		  transition(this, AL_ALARM_EXPIRED_STATE);
		  this->alarmState = ALARM_EXPIRED;
		  }
		*/
		break;
		
	default:
		break;
	}
}

static void alarmExpiredState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
}

static void alarmSetHoursState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (sig) {

	case AL_INC_SIG:
		EWatchTimeset_Dispatch(&this->expirationTime, TS_INC_SIG);
		break;

	case AL_DEC_SIG:
		EWatchTimeset_Dispatch(&this->expirationTime, TS_DEC_SIG);
		break;

	case AL_ALARM_SET_SIG:
		EWatchTimeset_Dispatch(&this->expirationTime, TS_TOGGLE_MODE_SIG);
		transition(this, alarmSetMinutesState);
		break;

	default:
		break;
	}
}

static void alarmSetMinutesState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (sig) {

	case AL_INC_SIG:
		EWatchTimeset_Dispatch(&this->expirationTime, TS_INC_SIG);
		break;

	case AL_DEC_SIG:
		EWatchTimeset_Dispatch(&this->expirationTime, TS_DEC_SIG);
		break;

	case AL_ALARM_SET_SIG:
		transition(this, alarmOnState);
		this->alarmState = ALARM_ON;
		break;

	default:
		break;
	}
}


	/* switch (this->state) { */
	/* case AL_ALARM_OFF_STATE: */

	/* 	break; */

	
	/* 	break; */

	/* case AL_SET_ALARM_MINUTES_STATE: */
	/* 	}		 */

	/* 	break; */

	/* case AL_ALARM_ON_STATE: */

	/* 	break; */

	/* case AL_ALARM_EXPIRED_STATE: */
	/* 	break; */

	/* default: */
	/* 	break; */
	/* } */
