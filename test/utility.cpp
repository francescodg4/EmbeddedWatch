#include "utility.h"

/* int convertToTicks(int hours, int minutes, int seconds, int tenths) */
/* { */
/* 	int ticks = 0; */

/* 	ticks += hours * TENTHS_IN_HOUR; */
/* 	ticks += minutes * TENTHS_IN_MINUTE; */
/* 	ticks += seconds * TENTHS_IN_SECOND; */
/* 	ticks += tenths; */

/* 	return ticks; */
/* } */

void checkTime(
	int expectedHours,
	int expectedMinutes,
	int expectedSeconds,
	int expectedTenths,
	ClockCounter *c)
{
	// TEST_ASSERT_EQUAL(expectedHours, ClockCounter_GetHours(c));
	// TEST_ASSERT_EQUAL(expectedMinutes, ClockCounter_GetMinutes(c));
	// TEST_ASSERT_EQUAL(expectedSeconds, ClockCounter_GetSeconds(c));
	// TEST_ASSERT_EQUAL(expectedTenths, ClockCounter_GetTenths(c));
}

void setAlarmTo(EWatchAlarm *a, int hours, int minutes)
{	
	EWatchTimeset_Set(&a->expirationTime, 0);

	EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set hours
	   
	int i;
	for (i = 0; i < hours; i++)
		EWatchAlarm_Dispatch(a, AL_INC_SIG);	
	
	EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set minutes

	for (i = 0; i < minutes; i++)
		EWatchAlarm_Dispatch(a, AL_INC_SIG);
	
	EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set alarm
}
