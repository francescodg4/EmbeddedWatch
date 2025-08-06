#include <catch2/catch_all.hpp>

#include <EWatchAlarm.h>
#include "utility.h"

static EWatchAlarm alarm;
static ClockCounter external;

static void setAlarmTo(EWatchAlarm *a, int hours, int minutes)
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

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
	int ticks = convertToTicks(12, 0, 0, 0);
	int i;
	
	for (i = 0; i < ticks; i++) {
		ClockCounter_Tick(&external);
		EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG);
	}
}

static void setUp(void)
{	
	ClockCounter_Init(&external);
	EWatchAlarm_Init(&alarm, &external);	
}

TEST_CASE("test_InitializeWithDefaultAlarmAt12(void)")
{
	setUp();

	EWatchAlarm alarm;

	EWatchAlarm_Init(&alarm, &external);
	
	TEST_ASSERT_EQUAL(12, EWatchAlarm_GetHours(&alarm));
	TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
}

TEST_CASE("test_SetAlarmToTime(void)")
{
	setUp();

	setAlarmTo(&alarm, 11, 0);

	TEST_ASSERT_EQUAL(11, EWatchTimeset_GetHours(&alarm.expirationTime));
	TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
	TEST_ASSERT_EQUAL(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm));
}

TEST_CASE("test_SwicthAlarmStateOnWhenSignalReceived(void)")
{
	setUp();

	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	
	TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
	
	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG);
	TEST_ASSERT_EQUAL(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm));       
}

TEST_CASE("test_SetAlarmWaitForTicksAndExpire(void)")
{
	setUp();

	setAlarmTo(&alarm, 12, 0);

	enum AlarmState prevState = EWatchAlarm_GetAlarmState(&alarm);

	waitFor(12, 0, 0, 0);

	enum AlarmState currentState = EWatchAlarm_GetAlarmState(&alarm);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, prevState, "Expected ALARM_OFF");
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, currentState, "Expected ALARM_EXPIRED");
}

TEST_CASE("test_AlarmWillExpireIfExternalTimeIsChanged(void)")
{
	setUp();

	ClockCounter_Set(&external, convertToTenths(11, 59, 59, 9));
   
	setAlarmTo(&alarm, 12, 0);

	unsigned int twelve = EWatchTimeset_GetCount(&alarm.expirationTime);
	
	TEST_ASSERT_EQUAL(convertToTenths(12, 0, 0, 0), twelve);

	ClockCounter_Tick(&external);
	EWatchAlarm_Dispatch(&alarm, AL_CLOCK_TICK_SIG);
	
	enum AlarmState alarmState = EWatchAlarm_GetAlarmState(&alarm);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, alarmState, "Expected ALARM_EXPIRED");
}

TEST_CASE("test_AlarmIsNotYetExpired(void)")
{
	setUp();

	ClockCounter_Set(&external, convertToTenths(11, 59, 59, 8));

	waitFor(0, 0, 0, 1);

	enum AlarmState alarmState = EWatchAlarm_GetAlarmState(&alarm);
	
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_OFF, alarmState, "Expected ALARM_OFF");
}

TEST_CASE("test_SwitchAlarmOffWhenExpired(void)")
{
	setUp();

	ClockCounter_Set(&external, convertToTenths(5, 29, 0, 0));

	setAlarmTo(&alarm, 5, 30);
	
	waitFor(0, 1, 0, 0);

	enum AlarmState alarmState = EWatchAlarm_GetAlarmState(&alarm);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, alarmState, "Expected ALARM_EXPIRED");

	EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); // Shutdown
	alarmState = EWatchAlarm_GetAlarmState(&alarm);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_OFF, alarmState, "Expected ALARM_OFF");
}
