#include <catch2/catch_all.hpp>

#include <EWatchTimeset.h>
#include "utility.h"

static EWatchTimeset timeset;

static void setUp(void)
{
    EWatchTimeset_Init(&timeset);
}

TEST_CASE("Initilized at time all zero")
{
    EWatchTimeset w;
    EWatchTimeset_Init(&w);

    checkTime(0, 0, 0, 0, &w.internal);

    TEST_ASSERT_EQUAL_MESSAGE(TS_SET_HOURS_STATE, w.state, "Expected TS_SET_HOURS_STATE");
}

TEST_CASE("Add hour")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);

    checkTime(1, 0, 0, 0, &timeset.internal);
}

TEST_CASE("Add minutes")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);
    EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);

    checkTime(0, 1, 0, 0, &timeset.internal);
}

TEST_CASE("Add more hours")
{
    setUp();

    for (int i = 0; i < 5; i++) {
        EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
    }

    checkTime(5, 0, 0, 0, &timeset.internal);
}

TEST_CASE("Add more minutes")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);

    for (int i = 0; i < 50; i++) {
        EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
    }

    checkTime(0, 50, 0, 0, &timeset.internal);
}

TEST_CASE("Add minutes, then add hours")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);

    for (int i = 0; i < 30; i++) {
        EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
    }

    EWatchTimeset_Dispatch(&timeset, TS_SET_HOURS_MODE_SIG);

    for (int i = 0; i < 2; i++) {
        EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);
    }

    checkTime(2, 30, 0, 0, &timeset.internal);
}

TEST_CASE("Decrement hours")
{
    setUp();

    ClockCounter_Set(&timeset.internal, convertToTicks(2, 30, 0, 0));

    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);

    checkTime(1, 30, 0, 0, &timeset.internal);
}

TEST_CASE("Decrement at limit")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);
    checkTime(23, 0, 0, 0, &timeset.internal);

    ClockCounter_Set(&timeset.internal, convertToTicks(1, 0, 0, 0));

    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);
    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);

    checkTime(23, 0, 0, 0, &timeset.internal);
}

TEST_CASE("Decrement minutes")
{
    setUp();

    ClockCounter_Set(&timeset.internal, convertToTicks(1, 23, 0, 0));

    EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);
    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);

    checkTime(1, 22, 0, 0, &timeset.internal);

    for (int i = 0; i < 23; i++) {
        EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);
    }

    checkTime(0, 59, 0, 0, &timeset.internal);
}

TEST_CASE("Decrement minutes at limit")
{
    setUp();

    EWatchTimeset_Dispatch(&timeset, TS_SET_MINUTES_MODE_SIG);
    EWatchTimeset_Dispatch(&timeset, TS_DEC_SIG);

    checkTime(23, 59, 0, 0, &timeset.internal);
}

TEST_CASE("Set timeset")
{
    setUp();

    int ticks = convertToTicks(2, 30, 0, 0);
    EWatchTimeset_Set(&timeset, ticks);

    TEST_ASSERT_EQUAL(ticks, EWatchTimeset_GetCount(&timeset));
}

TEST_CASE("Set time and show hours, minutes, and seconds")
{
    setUp();

    EWatchTimeset_Set(&timeset, convertToTicks(2, 32, 23, 1));

    checkTime(2, 32, 23, 1, &timeset.internal);
}

TEST_CASE("Set time and add hour")
{
    setUp();

    EWatchTimeset_Set(&timeset, convertToTicks(2, 33, 22, 1));

    EWatchTimeset_Dispatch(&timeset, TS_INC_SIG);

    TEST_ASSERT_EQUAL(3, EWatchTimeset_GetHours(&timeset));
    TEST_ASSERT_EQUAL(33, EWatchTimeset_GetMinutes(&timeset));
    TEST_ASSERT_EQUAL(22, EWatchTimeset_GetSeconds(&timeset));
    TEST_ASSERT_EQUAL(1, EWatchTimeset_GetTenths(&timeset));
}
