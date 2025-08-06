#include <catch2/catch_all.hpp>

#include <EWatchAlarm.h>
#include "utility.h"

static EWatchAlarm alarm;
static ClockCounter external;

static void setAlarmTo(EWatchAlarm* a, int hours, int minutes)
{
    EWatchTimeset_Set(&a->expirationTime, 0);

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set hours

    int i;
    for (i = 0; i < hours; i++) {
        EWatchAlarm_Dispatch(a, AL_INC_SIG);
    }

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set minutes

    for (i = 0; i < minutes; i++) {
        EWatchAlarm_Dispatch(a, AL_INC_SIG);
    }

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set alarm
}

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
    int ticks = utils::convertToTicks(12, 0, 0, 0);
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

TEST_CASE("Initialize with default alarm at 12:00", "[alarm]")
{
    EWatchAlarm alarm;

    EWatchAlarm_Init(&alarm, &external);

    TEST_ASSERT_EQUAL(12, EWatchAlarm_GetHours(&alarm));
    TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
    TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm));
}

TEST_CASE("Set alarm to time", "[alarm]")
{
    setUp();

    setAlarmTo(&alarm, 11, 0);

    TEST_ASSERT_EQUAL(11, EWatchTimeset_GetHours(&alarm.expirationTime));
    TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&alarm));
    TEST_ASSERT_EQUAL(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm));
}

TEST_CASE("Switch AlarmState to 'on' when hour and minutes are set, and an additional confirmation signal is received", "[alarm]")
{
    setUp();

    EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); // set hours
    EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); // set minutes

    TEST_ASSERT_EQUAL(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm)); // alarm is still 'off'

    EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); // confirm alarm
    TEST_ASSERT_EQUAL(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm)); // alarm is 'on'
}

TEST_CASE("Set alarm, wait for ticks, and expire", "[alarm]")
{
    setUp();

    setAlarmTo(&alarm, 12, 0);

    TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, EWatchAlarm_GetAlarmState(&alarm), "Expected ALARM_OFF");

    waitFor(12, 0, 0, 0);

    TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, EWatchAlarm_GetAlarmState(&alarm), "Expected ALARM_EXPIRED");
}

TEST_CASE("Alarm will expire if external time is changed", "[alarm]")
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

TEST_CASE("Once alarm expires, an AlarmSet signal turns off the alarm", "[alarm]")
{
    setUp();

    ClockCounter_Set(&external, convertToTenths(5, 29, 0, 0));

    setAlarmTo(&alarm, 5, 30);

    waitFor(0, 1, 0, 0);

    TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, EWatchAlarm_GetAlarmState(&alarm), "Expected ALARM_EXPIRED");

    EWatchAlarm_Dispatch(&alarm, AL_ALARM_SET_SIG); // Shutdown

    TEST_ASSERT_EQUAL_MESSAGE(ALARM_OFF, EWatchAlarm_GetAlarmState(&alarm), "Expected ALARM_OFF");
}
