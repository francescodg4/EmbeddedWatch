#include "unity.h"
#include "../EWatchTimeset.h"
#include "utility.h"

static EWatchTimeset w;

void setUp(void)
{
	EWatchTimeset_Init(&w);
}

void test_InitilizedAtTime0(void)
{
	EWatchTimeset w;
	EWatchTimeset_Init(&w);

	checkTime(0, 0, 0, 0, &w.internal);
	TEST_ASSERT_EQUAL_MESSAGE(TS_SET_HOURS_STATE, w.state, "Expected TS_SET_HOURS_STATE");
}

void test_AddHour(void)
{
	EWatchTimeset_Dispatch(&w, TS_INC_HOURS_SIG);
	
	checkTime(1, 0, 0, 0, &w.internal);
}

void test_AddMinute(void)
{
	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	EWatchTimeset_Dispatch(&w, TS_INC_MINUTES_SIG);
	
	checkTime(0, 1, 0, 0, &w.internal);
}


void test_AddMoreHours(void)
{
	int i;

	for (i = 0; i < 5; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_HOURS_SIG);

	checkTime(5, 0, 0, 0, &w.internal);
}

void test_AddMoreMinutes(void)
{
	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	
	int i;
	for (i = 0; i < 50; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_MINUTES_SIG);

	checkTime(0, 50, 0, 0, &w.internal);	
}

