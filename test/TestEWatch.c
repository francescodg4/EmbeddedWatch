#include "unity.h"
#include "../EWatch.h"

static EWatch watch;

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
	EWatch_Dispatch(&watch, CLOCK_TICK);
	TEST_ASSERT_EQUAL(1, EWatch_GetTenths(&watch));
}

void test_TickOneSecondWhenTenthsArePassed(void)
{
	int i;
	for (i = 0; i < 10; i++)
		EWatch_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetSeconds(&watch));
}

void test_TickOneMinuteWhenSecondsArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 10; i++)
		EWatch_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetMinutes(&watch));
}

void test_TickOneHourWhenMinutesArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 60 * 10; i++)
		EWatch_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetHours(&watch));
}

void test_ResetWhenADayIsPassed(void)
{
	int day = 24 * 60 * 60 * 10;
	
	int i;
	for (i = 0; i < day; i++)
		EWatch_Dispatch(&watch, CLOCK_TICK);
	
	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetHours(&watch));	
}

void test_CountShouldNotBeMoreThanDayLength(void)
{
	int i;
	for (i = 0; i < TENTHS_IN_DAY; i++)
		EWatch_Dispatch(&watch, CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, watch.counter);
}
