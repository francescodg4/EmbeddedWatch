#include <catch2/catch_all.hpp>

#include <EWatchTimeset.h>
#include "utility.h"

static EWatchTimeset w;

void setUp(void)
{
	EWatchTimeset_Init(&w);
}

TEST_CASE("InitilizedAtTime0(void)")
{
	EWatchTimeset w;
	EWatchTimeset_Init(&w);

	checkTime(0, 0, 0, 0, &w.internal);
	TEST_ASSERT_EQUAL_MESSAGE(TS_SET_HOURS_STATE, w.state, "Expected TS_SET_HOURS_STATE");
}

TEST_CASE("AddHour(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(1, 0, 0, 0, &w.internal);
}

TEST_CASE("AddMinute(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(0, 1, 0, 0, &w.internal);
}

TEST_CASE("AddMoreHours(void)")
{
	setUp();

	int i;

	for (i = 0; i < 5; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);

	checkTime(5, 0, 0, 0, &w.internal);
}

TEST_CASE("AddMoreMinutes(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	
	int i;
	for (i = 0; i < 50; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);

	checkTime(0, 50, 0, 0, &w.internal);	
}

TEST_CASE("AddMinutesThanAddHours(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	int i;
	for (i = 0; i < 30; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	EWatchTimeset_Dispatch(&w, TS_SET_HOURS_MODE_SIG);

	for (i = 0; i < 2; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(2, 30, 0, 0, &w.internal);
}

TEST_CASE("DecrementHours(void)")
{
	setUp();

	int time = convertToTicks(2, 30, 0, 0);
	ClockCounter_Set(&w.internal, time);

	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	
	checkTime(1, 30, 0, 0, &w.internal);
}

TEST_CASE("DecrementAtLimit(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	checkTime(23, 0, 0, 0, &w.internal);

	int anHour = convertToTicks(1, 0, 0, 0);
	ClockCounter_Set(&w.internal, anHour);

	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	
	checkTime(23, 0, 0, 0, &w.internal);
}

TEST_CASE("DecrementMinutes(void)")
{
	setUp();

	int secureTime = convertToTicks(1, 23, 0, 0);	
	ClockCounter_Set(&w.internal, secureTime);

	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);

	checkTime(1, 22, 0, 0, &w.internal);

	int i;
	for (i = 0; i < 23; i++)
		EWatchTimeset_Dispatch(&w, TS_DEC_SIG);

	checkTime(0, 59, 0, 0, &w.internal);
}

TEST_CASE("DecrementMinutesAtLimit(void)")
{
	setUp();

	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);

	checkTime(23, 59, 0, 0, &w.internal);
}

TEST_CASE("SetTimeset(void)")
{
	setUp();

	int ticks = convertToTicks(2, 30, 0, 0);
	EWatchTimeset_Set(&w, ticks);

	TEST_ASSERT_EQUAL(ticks, EWatchTimeset_GetCount(&w));
}

TEST_CASE("SetTimeAndShowHoursMinutesAndSeconds(void)")
{
	setUp();

	int ticks = convertToTicks(2, 32, 23, 1);
	EWatchTimeset_Set(&w, ticks);
	
	checkTime(2, 32, 23, 1, &w.internal);
}

TEST_CASE("SetTimeAndAddHour(void)")
{
	setUp();

	int ticks = convertToTicks(2, 33, 22, 1);
	EWatchTimeset_Set(&w, ticks);
	
	EWatchTimeset_Dispatch(&w, TS_INC_SIG);

	TEST_ASSERT_EQUAL(3, EWatchTimeset_GetHours(&w));
	TEST_ASSERT_EQUAL(33, EWatchTimeset_GetMinutes(&w));	
	TEST_ASSERT_EQUAL(22, EWatchTimeset_GetSeconds(&w));
	TEST_ASSERT_EQUAL(1, EWatchTimeset_GetTenths(&w));
}
