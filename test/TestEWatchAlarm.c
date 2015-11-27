#include "unity.h"
#include "../EWatchAlarm.h"

static EWatchAlarm alarm;
static ClockCounter external;

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
	int ticks = convertToTicks(12, 0, 0, 0);
	int i;
	
	for (i = 0; i < ticks; i++) {
		ClockCounter_Tick(&external);
		EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG);
	}
}

void setUp(void)
{	
	ClockCounter_Init(&external);
	EWatchAlarm_Init(&alarm, &external);	
}

void test_InitializeWithDefaultAlarmAt12(void)
{
	EWatchAlarm alarm;

	EWatchAlarm_Init(&alarm, &external);
	
	TEST_ASSERT_EQUAL(12, EWatchAlarm_GetHours(&alarm));
	TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
}

void test_SwicthAlarmStateOnWhenSignalReceived(void)
{
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
	
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	TEST_ASSERT_EQUAL(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm));       
}

/* void test_SetAlarmWaitForTicksAndExpire(void) */
/* { */
/* 	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); */

/* 	enum AlarmState prevState = EWatchAlarm_GetAlarmState(&alarm);	 */

/* 	waitFor(12, 0, 0, 0); */

/* 	enum AlarmState currentState = EWatchAlarm_GetAlarmState(&alarm); */

/* 	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, prevState, "Expected ALARM_OFF"); */
/* 	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, currentState, "Expected ALARM_EXPIRED"); */
/* } */

/* void test_AlarmWillExpireIfExternalTimeIsChanged(void) */
/* { */
/* 	ClockCounter_Set(&external, convertToTenths(11, 59, 59, 9)); */

/* 	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); */
       
/* 	ClockCounter_Tick(&external); */
/* 	EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG); */
	
/* 	enum AlarmState alarmState = EWatchAlarm_GetAlarmState(&alarm); */

/* 	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, alarmState, "Expected ALARM_EXPIRED"); */
/* } */

/* void test_AlarmIsNotYetExpired(void) */
/* { */
/* 	ClockCounter_Set(&external, convertToTenths(11, 59, 59, 8)); */
	
/* 	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); */
       
/* 	ClockCounter_Tick(&external); */
/* 	EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG); */
	
/* 	enum AlarmState alarmState = EWatchAlarm_GetAlarmState(&alarm); */
	
/* 	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, alarmState, "Expected ALARM_ON"); */
/* } */

/* void test_SetAlarmTo11am(void) */
/* { */
/* 	EWatchAlarm_Dispatch(&alarm,  */
//}
