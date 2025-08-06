#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ClockCounter.h>

#ifdef __cplusplus
}
#endif

namespace utils {

void checkTime(
    int expectedHours,
    int expectedMinutes,
    int expectedSeconds,
    int expectedTenths,
    ClockCounter* c);

inline int convertToTicks(int hours, int minutes, int seconds, int tenths)
{
    return convertToTenths(hours, minutes, seconds, tenths);
}

} // namespace utils

#define TEST_ASSERT_EQUAL(expected, actual) \
    REQUIRE(expected == actual);

#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message) \
    INFO(message);                                           \
    REQUIRE(expected == actual);

#endif /* UTILITY_H */
