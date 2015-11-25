#include "unity.h"
#include "../EWatchTimeset.h"
#include "utility.h"

void test_InitilizedAtTime0(void)
{
	EWatchTimeset w;
	EWatchTimeset_Init(&w);

	checkTime(0, 0, 0, 0, &w.internal);
}

void test_AddHour(void)
{
	TEST_IGNORE();
}

void test_AddMinute(void)
{
	TEST_IGNORE();
}

void test_AddMoreMinutes(void)
{
	TEST_IGNORE();
}

void test_AddMoreHours(void)
{
	TEST_IGNORE();
}
