#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ClockCounter.h"
#include "../EWatchAlarm.h"

#ifdef __cplusplus
}
#endif

int convertToTicks(int hours, int minutes, int seconds, int tenths);
void setAlarmTo(EWatchAlarm *a, int hours, int minutes);

void checkTime(
	int expectedHours,
	int expectedMinutes,
	int expectedSeconds,
	int expectedTenths,
	ClockCounter *c);

#define TEST_ASSERT_EQUAL(expected, actual) \
    REQUIRE(expected == actual);

#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message) \
    INFO(message);                                           \
    REQUIRE(expected == actual);

#endif /* UTILITY_H */
