#include "unity.h"
#include "../EWatch.h"
#include "utility.h"

static EWatch watch;
const enum EWatchSignal CLOCK_UPDATE = -1;

void setUp(void)
{
	EWatch_Init(&watch);
}

void test_SignalsShouldNotBeEqual(void)
{
	const char *message = "CLOCK_TICK != SET_CLOCK_MODE";
	TEST_ASSERT_FALSE_MESSAGE(EW_CLOCK_TICK == EW_SET_CLOCK_MODE, message);
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
	EWatch_Dispatch(&watch, EW_CLOCK_TICK);
	TEST_ASSERT_EQUAL(1, EWatch_GetTenths(&watch));
}

void test_TickOneSecondWhenTenthsArePassed(void)
{
	int i;
	for (i = 0; i < 10; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetSeconds(&watch));
}

void test_TickOneMinuteWhenSecondsArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 10; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetMinutes(&watch));
}

void test_TickOneHourWhenMinutesArePassed(void)
{
	int i;
	for (i = 0; i < 60 * 60 * 10; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(1, EWatch_GetHours(&watch));
}

void test_ResetWhenADayIsPassed(void)
{
	int day = 24 * 60 * 60 * 10;

	int i;
	for (i = 0; i < day; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);
	
	TEST_ASSERT_EQUAL(0, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(0, EWatch_GetHours(&watch));	
}

void test_CountShouldNotBeMoreThanDayLength(void)
{
	int i;
	for (i = 0; i < TENTHS_IN_DAY; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

	TEST_ASSERT_EQUAL(0, watch.clock.counter);
}

void test_CountTo3Hours20MinutesAnd30Seconds(void)
{
	int ticks = convertToTicks(3, 20, 30, 0);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

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
		EWatch_Dispatch(&watch, EW_CLOCK_TICK);

	TEST_ASSERT_EQUAL(TENTHS_IN_DAY - 1, watch.clock.counter);
	TEST_ASSERT_EQUAL(23, EWatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(59, EWatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(59, EWatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(9, EWatch_GetTenths(&watch));
}

void test_SwitchToAlarmModeWhenSignalIsReceived(void)
{
	enum EWatchMode previous = EWatch_GetMode(&watch);

	EWatch_Dispatch(&watch, EW_SET_ALARM_MODE);
	EWatch_Dispatch(&watch, EW_CLOCK_TICK);
	
	TEST_ASSERT_EQUAL_MESSAGE(CLOCK_MODE, previous, "Expected CLOCK_MODE");     
	TEST_ASSERT_EQUAL(1, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_MODE, EWatch_GetMode(&watch), "Expected ALARM_MODE");
}

void test_SameSignalGoesIntoTwoHIerarchicalMachines(void)
{
	EWatch_Dispatch(&watch, EW_CLOCK_TICK);
	EWatch_Dispatch(&watch, EW_SET_ALARM_MODE);

	enum EWatchMode previous = EWatch_GetMode(&watch);

	EWatch_Dispatch(&watch, EW_CLOCK_TICK);
	EWatch_Dispatch(&watch, EW_SET_CLOCK_MODE);

	TEST_ASSERT_EQUAL(2, EWatch_GetTenths(&watch));
	TEST_ASSERT_EQUAL_MESSAGE(CLOCK_MODE, EWatch_GetMode(&watch), "Expected CLOCK_MODE");
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_MODE, previous, "Expected ALARM_MODE");
}
