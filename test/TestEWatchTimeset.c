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
	EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(1, 0, 0, 0, &w.internal);
}

void test_AddMinute(void)
{
	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(0, 1, 0, 0, &w.internal);
}

void test_AddMoreHours(void)
{
	int i;

	for (i = 0; i < 5; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);

	checkTime(5, 0, 0, 0, &w.internal);
}

void test_AddMoreMinutes(void)
{
	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	
	int i;
	for (i = 0; i < 50; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);

	checkTime(0, 50, 0, 0, &w.internal);	
}

void test_AddMinutesThanAddHours(void)
{
	EWatchTimeset_Dispatch(&w, TS_SET_MINUTES_MODE_SIG);
	int i;
	for (i = 0; i < 30; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	EWatchTimeset_Dispatch(&w, TS_SET_HOURS_MODE_SIG);

	for (i = 0; i < 2; i++)
		EWatchTimeset_Dispatch(&w, TS_INC_SIG);
	
	checkTime(2, 30, 0, 0, &w.internal);
}

void test_DecrementHours(void)
{
	int time = convertToTicks(2, 30, 0, 0);
	ClockCounter_Set(&w.internal, time);

	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	
	checkTime(1, 30, 0, 0, &w.internal);
}

void test_DecrementAtLimit(void)
{
	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	checkTime(23, 0, 0, 0, &w.internal);

	int anHour = convertToTicks(1, 0, 0, 0);
	ClockCounter_Set(&w.internal, anHour);

	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	EWatchTimeset_Dispatch(&w, TS_DEC_SIG);
	
	checkTime(23, 0, 0, 0, &w.internal);
}

void test_DecrementMinutes(void)
{
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
