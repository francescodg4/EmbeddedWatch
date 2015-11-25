#include "unity.h"
#include "../EWatchClock.h"
#include "utility.h"

static EWatchClock watch;

void setUp(void)
{
	EWatchClock_Init(&watch);
}

void test_StartWatchWithInitialState(void)
{
	EWatchClock watch;
	
	EWatchClock_Init(&watch);
	
	TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
}

void test_SendClockTickSignalToWatch(void)
{
	EWatchClock_Dispatch(&watch, CLOCK_TICK);
	TEST_ASSERT_EQUAL(1, EWatchClock_GetTenths(&watch));
}

void test_TickOneSecondWhenTenthsArePassed(void)
{
	int i;
	for (i = 0; i < 10; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
	TEST_ASSERT_EQUAL(1, EWatchClock_GetSeconds(&watch));
}

void test_TickOneMinuteWhenSecondsArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 10; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(1, EWatchClock_GetMinutes(&watch));
}

void test_TickOneHourWhenMinutesArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 60 * 10; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(1, EWatchClock_GetHours(&watch));
}

void test_ResetWhenADayIsPassed(void)
{
	int day = 24 * 60 * 60 * 10;

	int i;
	for (i = 0; i < day; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);
	
	TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
}

void test_CountShouldNotBeMoreThanDayLength(void)
{
	int i;
	for (i = 0; i < TENTHS_IN_DAY; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, ClockCounter_GetCount(&watch.counter));
}

void test_CountTo3Hours20MinutesAnd30Seconds(void)
{
	int ticks = convertToTicks(3, 20, 30, 0);

	int i;
	for (i = 0; i < ticks; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(3, EWatchClock_GetHours(&watch));
	TEST_ASSERT_EQUAL(20, EWatchClock_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(30, EWatchClock_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
}

void test_CountToDayLimit(void)
{
	int ticks = convertToTicks(23, 59, 59, 9);
	
	int i;
	for (i = 0; i < ticks; i++)
		EWatchClock_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(TENTHS_IN_DAY - 1, ClockCounter_GetCount(&watch.counter));
	TEST_ASSERT_EQUAL(23, EWatchClock_GetHours(&watch));
	TEST_ASSERT_EQUAL(59, EWatchClock_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(59, EWatchClock_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(9, EWatchClock_GetTenths(&watch));
}
