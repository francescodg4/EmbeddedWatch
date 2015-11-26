#include "EWatchAlarm.h"

static void transition(EWatchAlarm *this, enum EWatchAlarmState state);

void EWatchAlarm_Init(EWatchAlarm *this)
{
	unsigned int defaultTime = convertToTenths(12, 0, 0, 0);

	ClockCounter_Init(&this->alarmCounter);
	ClockCounter_Set(&this->alarmCounter, defaultTime);
	ClockCounter_Set(&this->finalCounter, defaultTime);

	this->alarmState = ALARM_OFF;
	
	transition(this, AL_ALARM_OFF_STATE);
}

void EWatchAlarm_Init2(EWatchAlarm *this, ClockCounter *external)
{
	unsigned int defaultTime = convertToTenths(12, 0, 0, 0);

// 	ClockCounter_Init(&this->alarmCounter);
//	ClockCounter_Set(&this->alarmCounter, defaultTime);
	ClockCounter_Set(&this->finalCounter, defaultTime);
	// ClockCounter_Set(&this->alarmCounter, defaultTime);
	this->external = external;	

	this->alarmState = ALARM_OFF;

	transition(this, AL_ALARM_OFF_STATE);
}


void EWatchAlarm_Dispatch(EWatchAlarm *this, enum EWatchAlarmSignal sig)
{
	switch (this->state) {
	case AL_ALARM_OFF_STATE:
		switch (sig) {
		case AL_ALARM_SET_SIG:
			transition(this, AL_ALARM_ON_STATE);
			this->alarmState = ALARM_ON;
			break;

		default:
			break;
		}
		break;

	case AL_ALARM_ON_STATE:
		switch (sig) {
		case AL_CLOCK_TICK_SIG:
			if (ClockCounter_GetCount(this->external) == 
			    ClockCounter_GetCount(&this->finalCounter)) {
				transition(this, AL_ALARM_EXPIRED_STATE);
				this->alarmState = ALARM_EXPIRED;
			}
			break;
		
		default:
			break;
		}

		break;

	case AL_ALARM_EXPIRED_STATE:
		break;

	default:
		break;
	}
}

int EWatchAlarm_GetHours(EWatchAlarm *this)
{
	return ClockCounter_GetHours(&this->finalCounter);
}

int EWatchAlarm_GetMinutes(EWatchAlarm *this)
{
	return ClockCounter_GetMinutes(&this->finalCounter);
}

enum AlarmState EWatchAlarm_GetAlarmState(EWatchAlarm *this)
{
	return this->alarmState;
}

static void transition(EWatchAlarm *this, enum EWatchAlarmState state)
{
	this->state = state;
}
