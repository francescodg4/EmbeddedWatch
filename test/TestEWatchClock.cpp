#include <catch2/catch_all.hpp>

#include <EWatchClock.h>
#include "utility.h"

static EWatchClock watch;

TEST_CASE("test_StartWatchWithInitialState(void)")
{
    EWatchClock watch;

    EWatchClock_Init(&watch);

    TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
}

TEST_CASE("test_SendClockTickSignalToWatch(void)")
{
    EWatchClock_Init(&watch);

    EWatchClock_Dispatch(&watch, CLOCK_TICK);
    TEST_ASSERT_EQUAL(1, EWatchClock_GetTenths(&watch));
}

TEST_CASE("test_TickOneSecondWhenTenthsArePassed(void)")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetSeconds(&watch));
}

TEST_CASE("test_TickOneMinuteWhenSecondsArePassed(void)")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 60 * 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetMinutes(&watch));
}

TEST_CASE("test_TickOneHourWhenMinutesArePassed(void)")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 60 * 60 * 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetHours(&watch));
}

TEST_CASE("test_ResetWhenADayIsPassed(void)")
{
    EWatchClock_Init(&watch);

    int day = 24 * 60 * 60 * 10;

    int i;
    for (i = 0; i < day; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
}

TEST_CASE("test_CountShouldNotBeMoreThanDayLength(void)")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < TENTHS_IN_DAY; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, ClockCounter_GetCount(&watch.counter));
}

TEST_CASE("test_CountTo3Hours20MinutesAnd30Seconds(void)")
{
    EWatchClock_Init(&watch);

    int ticks = convertToTicks(3, 20, 30, 0);

    int i;
    for (i = 0; i < ticks; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(3, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(20, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(30, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
}

TEST_CASE("test_CountToDayLimit(void)")
{
    EWatchClock_Init(&watch);

    int ticks = convertToTicks(23, 59, 59, 9);

    int i;
    for (i = 0; i < ticks; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(TENTHS_IN_DAY - 1, ClockCounter_GetCount(&watch.counter));
    TEST_ASSERT_EQUAL(23, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(59, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(59, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(9, EWatchClock_GetTenths(&watch));
}
