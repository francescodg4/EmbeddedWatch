#include "unity.h"
#include "../EWatchStopwatch.h"
#include "utility.h"

static EWatchStopwatch watch;

static void checkTime(int expectedHours, int expectedMinutes, int expectedSeconds, int expectedTenths, EWatchStopwatch *w)
{
	TEST_ASSERT_EQUAL(expectedHours, EWatchStopwatch_GetHours(w));
	TEST_ASSERT_EQUAL(expectedMinutes, EWatchStopwatch_GetMinutes(w));
	TEST_ASSERT_EQUAL(expectedSeconds, EWatchStopwatch_GetSeconds(w));
	TEST_ASSERT_EQUAL(expectedTenths, EWatchStopwatch_GetTenths(w));
}

void setUp(void)
{
	EWatchStopwatch_Init(&watch);
}

void test_InitializedCounterAtZero(void)
{
	EWatchStopwatch stopW;
	
	EWatchStopwatch_Init(&stopW);
  
	checkTime(0, 0, 0, 0, &stopW);
	TEST_ASSERT_EQUAL(ST_STOP_STATE, stopW.state);
}


void test_RunFor5Tenths(void)
{
	int i;
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	for (i = 0; i < 5; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	TEST_ASSERT_EQUAL(5, EWatchStopwatch_GetTenths(&watch));
}

void test_UpdateAccordingToClock(void)
{
	int ticks = convertToTicks(3, 30, 20, 2);
	int i;
	
	// Start stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	// Updates counter when clock tick is received
	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	TEST_ASSERT_EQUAL(3, EWatchStopwatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(30, EWatchStopwatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(20, EWatchStopwatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(2, EWatchStopwatch_GetTenths(&watch));
}

void test_StartStopwatchWaitForTimeAndStopNextClockEventsAreDiscarded(void)
{
	int ticks = convertToTicks(6, 20, 10, 5);
	int i;
	
	// Start stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	// Updates counter when clock tick is received
	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	// Stop stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	// Subsequent events are discarded
	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);
	
	TEST_ASSERT_EQUAL(6, EWatchStopwatch_GetHours(&watch));
	TEST_ASSERT_EQUAL(20, EWatchStopwatch_GetMinutes(&watch));
	TEST_ASSERT_EQUAL(10, EWatchStopwatch_GetSeconds(&watch));
	TEST_ASSERT_EQUAL(5, EWatchStopwatch_GetTenths(&watch));	
}

void test_RestartFromWereLeft(void)
{
	int ticks = convertToTicks(3, 30, 20, 2);
	int i;
  
	// Start stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);
	
	// Stop stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	// Restart stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	checkTime(3, 30, 20, 2, &watch);
}

void test_ResetWhenResetSignalIsReceived(void)
{
	int ticks = convertToTicks(1, 10, 11, 0);
	int i;

	// Start
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	// Stop
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);
	
	// Reset
	EWatchStopwatch_Dispatch(&watch, ST_RESET_SIG);
	
	checkTime(0, 0, 0, 0, &watch);
}

void test_ResetDoesNothingWhenInRunningState(void)
{
	int ticks = convertToTicks(1, 10, 11, 0);
	int i;

	// Start
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);
	
	// Send set of spurious Reset signals
	for (i = 0; i < 6; i++)
		EWatchStopwatch_Dispatch(&watch, ST_RESET_SIG);
	
	// Stop
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);
	
	checkTime(1, 10, 11, 0, &watch);
}

void test_DoubleToggleReturnsToStop(void)
{
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	TEST_ASSERT_EQUAL_MESSAGE(ST_STOP_STATE, watch.state, "Expected ST_STOP_STATE");
}
