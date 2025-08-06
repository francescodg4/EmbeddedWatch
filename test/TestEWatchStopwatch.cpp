#include <catch2/catch_all.hpp>

#include <EWatchStopwatch.h>
#include "utility.h"

static EWatchStopwatch watch;

static void setUp(void)
{
	EWatchStopwatch_Init(&watch);
}

TEST_CASE("test_InitializedCounterAtZero(void)")
{
	EWatchStopwatch stopW;
	
	EWatchStopwatch_Init(&stopW);
  
	checkTime(0, 0, 0, 0, &stopW.internal);
	TEST_ASSERT_EQUAL(ST_STOP_STATE, stopW.state);
}

TEST_CASE("test_WhenInStopStateDiscardClockEvents(void)")
{
	setUp();

	int i;
	
	for (i = 0; i < 100; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);
	
	TEST_ASSERT_EQUAL(0, EWatchStopwatch_GetTenths(&watch));
}

TEST_CASE("test_RunFor5Tenths(void)")
{
	setUp();

	int i;
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	for (i = 0; i < 5; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	TEST_ASSERT_EQUAL(5, EWatchStopwatch_GetTenths(&watch));
}

TEST_CASE("test_UpdateAccordingToClock(void)")
{
	setUp();

	int ticks = convertToTicks(3, 30, 20, 2);
	int i;
	
	// Start stopwatch
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	// Updates counter when clock tick is received
	for (i = 0; i < ticks; i++)
		EWatchStopwatch_Dispatch(&watch, ST_CLOCK_TICK_SIG);

	checkTime(3, 30, 20, 2, &watch.internal);
}

TEST_CASE("test_StartStopwatchWaitForTimeAndStopNextClockEventsAreDiscarded(void)")
{
	setUp();

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
	
	checkTime(6, 20, 10, 5, &watch.internal);	
}

TEST_CASE("test_RestartFromWereLeft(void)")
{
	setUp();

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

	checkTime(3, 30, 20, 2, &watch.internal);
}

TEST_CASE("test_ResetWhenResetSignalIsReceived(void)")
{
	setUp();

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
	
	checkTime(0, 0, 0, 0, &watch.internal);
}

TEST_CASE("test_ResetDoesNothingWhenInRunningState(void)")
{
	setUp();

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
	
	checkTime(1, 10, 11, 0, &watch.internal);
}

TEST_CASE("test_DoubleToggleReturnsToStop(void)")
{
	setUp();

	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);
	EWatchStopwatch_Dispatch(&watch, ST_TOGGLE_SIG);

	TEST_ASSERT_EQUAL_MESSAGE(ST_STOP_STATE, watch.state, "Expected ST_STOP_STATE");
}
