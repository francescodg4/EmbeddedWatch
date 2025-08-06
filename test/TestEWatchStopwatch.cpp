#include <catch2/catch_all.hpp>

#include <EWatchStopwatch.h>
#include "utility.h"

static EWatchStopwatch stopwatch;

static void setUp(void)
{
    EWatchStopwatch_Init(&stopwatch);
}

TEST_CASE("Initialized stopwatch all zero", "[stopwatch]")
{
    EWatchStopwatch stopW;

    EWatchStopwatch_Init(&stopW);

    utils::checkTime(0, 0, 0, 0, &stopW.internal);
    TEST_ASSERT_EQUAL(ST_STOP_STATE, stopW.state);
}

TEST_CASE("When in StopState discard clock events", "[stopwatch]")
{
    setUp();

    for (int i = 0; i < 100; i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    TEST_ASSERT_EQUAL(0, EWatchStopwatch_GetTenths(&stopwatch));
}

TEST_CASE("Run for arbitrary amount of tenths", "[stopwatch]")
{
    setUp();

    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG); // start stopwatch

    for (int i = 0; i < 5; i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    TEST_ASSERT_EQUAL(5, EWatchStopwatch_GetTenths(&stopwatch));
}

TEST_CASE("Update stopwatch value as ClockTick signals are received", "[stopwatch]")
{
    setUp();

    // Start stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    // Updates counter when clock tick is received
    for (int i = 0; i < utils::convertToTicks(3, 30, 20, 2); i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    utils::checkTime(3, 30, 20, 2, &stopwatch.internal);
}

TEST_CASE("Once a running stopwatch is stopped, the next ClockTick events are ignored", "[stopwatch]")
{
    setUp();

    // Start stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    // Updates counter when clock tick is received
    for (int i = 0; i < utils::convertToTicks(6, 20, 10, 5); i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    // Stop stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    // Subsequent events are discarded
    for (int i = 0; i < 458; i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    utils::checkTime(6, 20, 10, 5, &stopwatch.internal);
}

TEST_CASE("If the stopwatch is restarted, it resumes from the point it was paused.", "[stopwatch]")
{
    setUp();

    // Start stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    for (int i = 0; i < utils::convertToTicks(3, 30, 20, 2); i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    // Stop stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    // Restart stopwatch
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    utils::checkTime(3, 30, 20, 2, &stopwatch.internal);

    // ClickTick signals are now used by stopwatch
    for (int i = 0; i < 2; i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    utils::checkTime(3, 30, 20, 4, &stopwatch.internal);
}

TEST_CASE("Stopwatch resets when ResetSignal is received", "[stopwatch]")
{
    setUp();

    // Start
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    for (int i = 0; i < utils::convertToTicks(1, 10, 11, 0); i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    // Stop
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    // Reset
    EWatchStopwatch_Dispatch(&stopwatch, ST_RESET_SIG);

    utils::checkTime(0, 0, 0, 0, &stopwatch.internal);
}

TEST_CASE("Reset signals are ignored if the stopwatch is running", "[stopwatch]")
{
    setUp();

    // Start
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    for (int i = 0; i < utils::convertToTicks(1, 10, 11, 0); i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
    }

    // Send a set of spurious 'Reset' signals
    for (int i = 0; i < 6; i++) {
        EWatchStopwatch_Dispatch(&stopwatch, ST_RESET_SIG);
    }

    // Stop
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    utils::checkTime(1, 10, 11, 0, &stopwatch.internal);
}

TEST_CASE("Double toggle returns to stop", "[stopwatch]")
{
    setUp();

    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);
    EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

    TEST_ASSERT_EQUAL_MESSAGE(ST_STOP_STATE, stopwatch.state, "Expected ST_STOP_STATE");
}
