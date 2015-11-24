#include "unity.h"
#include "../EWatch.h"

const unsigned int CLOCK_UPDATE[2] = {CLOCK_TICK, 0};
static EWatch watch;

static int convertToTicks(int hours, int minutes, int seconds, int tenths)
{
	int ticks = 0;

	ticks += hours * TENTHS_IN_HOUR;
	ticks += minutes * TENTHS_IN_MINUTE;
	ticks += seconds * TENTHS_IN_SECOND;
	ticks += tenths;

	return ticks;
}

void setUp(void)
{
	EWatch_Init(&watch);
}

void test_StartWatchWithInitialState(void)
{
	EWatch watch;
	
	EWatch_Init(&watch);
	
	TEST_ASSERT_EQUAL(0, EWatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));	       
}

void test_SendClockTickSignalToWatch(void)
{
	EWatch_Dispatch(&watch, CLOCK_UPDATE);
	TEST_ASSERT_EQUAL(1, EWatch_GetTenths(&watch));
}

void test_TickOneSecondWhenTenthsArePassed(void)
{
	int i;
	for (i = 0; i < 10; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetSeconds(&watch));
}

void test_TickOneMinuteWhenSecondsArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 10; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetMinutes(&watch));
}

void test_TickOneHourWhenMinutesArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 60 * 10; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetHours(&watch));
}

void test_ResetWhenADayIsPassed(void)
{
	int day = 24 * 60 * 60 * 10;

	int i;
	for (i = 0; i < day; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);
	
	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetHours(&watch));	
}

void test_CountShouldNotBeMoreThanDayLength(void)
{
	int i;
	for (i = 0; i < TENTHS_IN_DAY; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(0, watch.clock.counter);
}

void test_CountTo3Hours20MinutesAnd30Seconds(void)
{
	int ticks = convertToTicks(3, 20, 30, 0);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(3, EWatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(20, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(30, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
}

void test_CountToDayLimit(void)
{
	int ticks = convertToTicks(23, 59, 59, 9);
	
	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, CLOCK_UPDATE);

	TEST_ASSERT_EQUAL(TENTHS_IN_DAY - 1, watch.clock.counter);
	TEST_ASSERT_EQUAL(23, EWatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(59, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(59, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(9, EWatch_GetTenths(&watch));
}

void test_SwitchToAlarmModeWhenSignalIsReceived(void)
{
	enum EWatchMode previous = EWatch_GetMode(&watch);
	unsigned int sig1[] = {0, SET_ALARM_MODE};

	EWatch_Dispatch(&watch, sig1);
	EWatch_Dispatch(&watch, CLOCK_UPDATE);
	
	TEST_ASSERT_EQUAL_MESSAGE(CLOCK_MODE, previous, "Should be CLOCK_MODE");     
	TEST_ASSERT_EQUAL(1, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_MODE, EWatch_GetMode(&watch), "Should be ALARM_MODE");
}

void test_SameSignalGoesIntoTwoHIerarchicalMachines(void)
{
	unsigned int sig1[] = {CLOCK_TICK, SET_CLOCK_MODE};
	unsigned int sig2[] = {CLOCK_TICK, SET_ALARM_MODE};

  	EWatch_Dispatch(&watch, sig1);
	EWatch_Dispatch(&watch, sig2);

	TEST_ASSERT_EQUAL(2, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(ALARM_MODE, EWatch_GetMode(&watch));
}
