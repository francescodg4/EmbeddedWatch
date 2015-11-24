#include "unity.h"
#include "EWatchAlarm.h"

void test_InitializeWithPreviouslySavedAlarm(void)
{
	EWatchAlarm alarm;

	EWatchAlarm_Init(&alarm);

	TEST_FAIL();
}
