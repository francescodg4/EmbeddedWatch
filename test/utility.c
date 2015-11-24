#include "utility.h"
#include "unity.h"
#include "../EWatchClock.h"

int convertToTicks(int hours, int minutes, int seconds, int tenths)
{
	int ticks = 0;

	ticks += hours * TENTHS_IN_HOUR;
	ticks += minutes * TENTHS_IN_MINUTE;
	ticks += seconds * TENTHS_IN_SECOND;
	ticks += tenths;

	return ticks;
}

void checkTime(
	int expectedHours,
	int expectedMinutes,
	int expectedSeconds,
	int expectedTenths,
	EWatchClock *w)
{
	TEST_ASSERT_EQUAL(expectedHours, EWatchClock_GetHours(w));
	TEST_ASSERT_EQUAL(expectedMinutes, EWatchClock_GetMinutes(w));
	TEST_ASSERT_EQUAL(expectedSeconds, EWatchClock_GetSeconds(w));
	TEST_ASSERT_EQUAL(expectedTenths, EWatchClock_GetTenths(w));
}
