#include "utility.h"
#include "unity.h"

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
	ClockCounter *w)
{
	TEST_ASSERT_EQUAL(expectedHours, ClockCounter_GetHours(w));
	TEST_ASSERT_EQUAL(expectedMinutes, ClockCounter_GetMinutes(w));
	TEST_ASSERT_EQUAL(expectedSeconds, ClockCounter_GetSeconds(w));
	TEST_ASSERT_EQUAL(expectedTenths, ClockCounter_GetTenths(w));
}
