#include "unity.h"
#include "../EWatchAlarm.h"

void test_InitializeWithDefaultAlarmAt12(void)
{
	EWatchAlarm alarm;
	EWatchAlarm_Init(&alarm);
	
	TEST_ASSERT_EQUAL(12, EWatchAlarm_GetHours(&alarm));
	TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
}

// void test_
