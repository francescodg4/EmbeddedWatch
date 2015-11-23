#include "unity.h"
#include "../EWatch.h"

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
	TEST_IGNORE();
}

void test_TickOneSecondWhenTenthsArePassed(void)
{
	TEST_IGNORE();
}

void test_TickOneMinuteWhenSecondsArePassed(void)
{
	TEST_IGNORE();
}

void test_TickOneHourWhenMinutesArePassed(void)
{
	TEST_IGNORE();
}

void test_ResetWhenADayIsPassed(void)
{
	TEST_IGNORE();
}

