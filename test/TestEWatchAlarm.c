#include "unity.h"
#include "../EWatchAlarm.h"

static EWatchAlarm alarm;

void setUp(void)
{
	EWatchAlarm_Init(&alarm);	
}

void test_InitializeWithDefaultAlarmAt12(void)
{
	EWatchAlarm alarm;
	EWatchAlarm_Init(&alarm);
	
	TEST_ASSERT_EQUAL(12, EWatchAlarm_GetHours(&alarm));
	TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
}

void test_SetAlarmWaitForTicksAndExpire(void)
{
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);

	enum AlarmState prevState = EWatchAlarm_GetAlarmState(&alarm);	

	int ticks = convertToTicks(12, 0, 0, 0);
	int i;
	
	for (i = 0; i < ticks; i++)
		EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG);
	
	enum AlarmState currentState = EWatchAlarm_GetAlarmState(&alarm);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, prevState, "Expected ALARM_OFF");
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, currentState, "Expected ALARM_EXPIRED");
}
