#include "EWatchAlarm.h"
#include "EWatchTimeset.h"

static void transition(EWatchAlarm *this, EWatchAlarmState state);

static void alarmOffState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmOnState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void alarmExpiredState(EWatchAlarm *this, enum EWatchAlarmSignal sig);

static void setHoursState(EWatchAlarm *this, enum EWatchAlarmSignal sig);
static void setMinutesState(EWatchAlarm *this, enum EWatchAlarmSignal sig);

void EWatchAlarm_Init(EWatchAlarm *this, ClockCounter *external)
{
	unsigned int defaultTime = convertToTenths(12, 0, 0, 0);

	EWatchTimeset_Init(&this->expirationTime);
	EWatchTimeset_Set(&this->expirationTime, defaultTime);

	this->external = external;	
	this->alarmState = ALARM_OFF;

	transition(this, alarmOffState);
}

void EWatchAlarm_Dispatch(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	(*this->state)(this, sig);
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
		transition(this, setHoursState);

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
	unsigned int prevExpirationTime;

	switch (sig) {
	case AL_CLOCK_TICK_SIG:
		  if (ClockCounter_GetCount(this->external) ==
		  EWatchTimeset_GetCount(&this->expirationTime)) {
			  transition(this, alarmExpiredState);
			  this->alarmState = ALARM_EXPIRED;
		  }
		  break;

	case AL_ALARM_SET_SIG:		
		transition(this, setHoursState);

		prevExpirationTime = EWatchTimeset_GetCount(&this->expirationTime);
		EWatchTimeset_Init(&this->expirationTime);
		EWatchTimeset_Set(&this->expirationTime, prevExpirationTime); 

		this->alarmState = ALARM_OFF;
		break;

	default:
		break;
	}
}

static void alarmExpiredState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (sig) {
	case AL_ALARM_SET_SIG:
		this->alarmState = ALARM_OFF;
		transition(this, alarmOffState);
		break;
	default:
		break;
	}
}

static void setHoursState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
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
		transition(this, setMinutesState);
		break;

	default:
		break;
	}
}

static void setMinutesState(EWatchAlarm *this, enum EWatchAlarmSignal sig)
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
