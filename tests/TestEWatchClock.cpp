#include <catch2/catch_all.hpp>

#include <EWatchClock.h>
#include "utility.h"

static EWatchClock watch;

TEST_CASE("Start watch with initial state", "[watch]")
{
    EWatchClock watch;

    EWatchClock_Init(&watch);

    TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
}

TEST_CASE("Send clock tick signal to watch", "[watch]")
{
    EWatchClock_Init(&watch);

    EWatchClock_Dispatch(&watch, CLOCK_TICK);

    TEST_ASSERT_EQUAL(1, EWatchClock_GetTenths(&watch));
}

TEST_CASE("Tick one second when 10 tenths are passed", "[watch]")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetSeconds(&watch));
}

TEST_CASE("Tick one minute when 60 seconds are passed", "[watch]")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 60 * 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetMinutes(&watch));
}

TEST_CASE("Tick one hour when 60 minutes are passed", "[watch]")
{
    EWatchClock_Init(&watch);

    int i;
    for (i = 0; i < 60 * 60 * 10; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(1, EWatchClock_GetHours(&watch));
}

TEST_CASE("Reset when 24 hours have passed", "[watch]")
{
    EWatchClock_Init(&watch);

    int day = 24 * 60 * 60 * 10;

    for (int i = 0; i < day; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, EWatchClock_GetTenths(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(0, EWatchClock_GetHours(&watch));
}

TEST_CASE("Count should not be more than 24 hours", "[watch]")
{
    EWatchClock_Init(&watch);

    for (int i = 0; i < TENTHS_IN_DAY; i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(0, ClockCounter_GetCount(&watch.counter));
}

TEST_CASE("Count to an arbitrary time", "[watch]")
{
    EWatchClock_Init(&watch);

    const int hours = 3;
    const int minutes = 20;
    const int seconds = 30;
    const int tenths = 9;

    for (int i = 0; i < utils::convertToTicks(hours, minutes, seconds, tenths); i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(hours, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(minutes, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(seconds, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(tenths, EWatchClock_GetTenths(&watch));
}

TEST_CASE("Count to day limit", "[watch]")
{
    EWatchClock_Init(&watch);

    for (int i = 0; i < utils::convertToTicks(23, 59, 59, 9); i++) {
        EWatchClock_Dispatch(&watch, CLOCK_TICK);
    }

    TEST_ASSERT_EQUAL(TENTHS_IN_DAY - 1, ClockCounter_GetCount(&watch.counter));
    TEST_ASSERT_EQUAL(23, EWatchClock_GetHours(&watch));
    TEST_ASSERT_EQUAL(59, EWatchClock_GetMinutes(&watch));
    TEST_ASSERT_EQUAL(59, EWatchClock_GetSeconds(&watch));
    TEST_ASSERT_EQUAL(9, EWatchClock_GetTenths(&watch));
}
