#include <catch2/catch_all.hpp>

#include <EWatchStopwatch.h>
#include "utility.h"

TEST_CASE("Stopwatch Basic Functionality", "[stopwatch]")
{
    EWatchStopwatch stopwatch;
    EWatchStopwatch_Init(&stopwatch);

    SECTION("Initialization sets all values to zero")
    {
        utils::checkTime(0, 0, 0, 0, &stopwatch.internal);
        CHECK(stopwatch.state == ST_STOP_STATE);
    }

    SECTION("In StopState, clock events are discarded")
    {
        for (int i = 0; i < 100; i++) {
            EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
        }
        CHECK(EWatchStopwatch_GetTenths(&stopwatch) == 0);
    }
}

TEST_CASE("Stopwatch State Transitions", "[stopwatch]")
{
    EWatchStopwatch stopwatch;
    EWatchStopwatch_Init(&stopwatch);

    SECTION("Running: Updates counter when clock ticks are received")
    {
        // Start stopwatch
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

        for (int i = 0; i < 5; i++) {
            EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
        }
        CHECK(EWatchStopwatch_GetTenths(&stopwatch) == 5);
    }

    SECTION("Running: Update stopwatch as ClockTick signals are received")
    {
        // Start stopwatch
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

        // Updates counter when clock tick is received
        for (int i = 0; i < utils::convertToTicks(3, 30, 20, 2); i++) {
            EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
        }

        utils::checkTime(3, 30, 20, 2, &stopwatch.internal);
    }

    SECTION("Toggle state between RUNNING and STOP")
    {
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);
        CHECK(stopwatch.state == ST_RUNNING_STATE);

        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);
        CHECK(stopwatch.state == ST_STOP_STATE);

        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);
        CHECK(stopwatch.state == ST_RUNNING_STATE);

        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);
        CHECK(stopwatch.state == ST_STOP_STATE);
    }

    SECTION("Once a RUNNING stopwatch is stopped, the next ClockTick events are ignored")
    {
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

    SECTION("If the stopwatch is restarted, it resumes from when it was paused")
    {
        // Start stopwatch
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

        for (int i = 0; i < utils::convertToTicks(3, 30, 20, 2); i++) {
            EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
        }

        // Stop stopwatch
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

        // Start stopwatch
        EWatchStopwatch_Dispatch(&stopwatch, ST_TOGGLE_SIG);

        utils::checkTime(3, 30, 20, 2, &stopwatch.internal);

        // ClickTick signals are now consumed by stopwatch
        for (int i = 0; i < 2; i++) {
            EWatchStopwatch_Dispatch(&stopwatch, ST_CLOCK_TICK_SIG);
        }

        utils::checkTime(3, 30, 20, 4, &stopwatch.internal);
    }

    SECTION("Stopwatch resets when RESET signal is received only if it has been stopped", "[stopwatch]")
    {
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

    SECTION("Reset signals are ignored if the stopwatch is running")
    {
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
}
