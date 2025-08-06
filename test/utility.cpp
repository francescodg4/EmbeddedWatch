#include "utility.h"
#include <catch2/catch_all.hpp>

namespace utils {

void checkTime(
    int expectedHours,
    int expectedMinutes,
    int expectedSeconds,
    int expectedTenths,
    ClockCounter* c)
{
    TEST_ASSERT_EQUAL(expectedHours, ClockCounter_GetHours(c));
    TEST_ASSERT_EQUAL(expectedMinutes, ClockCounter_GetMinutes(c));
    TEST_ASSERT_EQUAL(expectedSeconds, ClockCounter_GetSeconds(c));
    TEST_ASSERT_EQUAL(expectedTenths, ClockCounter_GetTenths(c));
}

} // namespace utils
