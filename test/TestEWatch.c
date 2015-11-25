#include "unity.h"
#include "../EWatch.h"
#include "utility.h"
#include <stdio.h>

#define OUT_SIZE 255

static EWatch watch;
static char out[OUT_SIZE];

static void output(EWatch *w, char *out)
{
	int mode = EWatch_GetMode(w);
	int hours = EWatch_GetHours(w);
	int minutes = EWatch_GetMinutes(w);
	int seconds = EWatch_GetSeconds(w);
	int tenths = EWatch_GetTenths(w);

	snprintf(out, OUT_SIZE, "Mode:%d %d:%d:%d %d", mode, hours, minutes, seconds, tenths);
}

void setUp(void)
{
	EWatch_Init(&watch);
}

void test_IntializationAsClockMode(void)
{
	EWatch watch;
	EWatch_Init(&watch);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:0 0:0:0 0", out);
}

void test_ReceivingClockTickEvent(void)
{
	int ticks = convertToTicks(13, 23, 22, 0);
	int i;

	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:0 13:23:22 0", out);	
}

void test_SwitchToStopwatchMode(void)
{
	int i;
	for (i = 0; i < 800; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:1 0:0:0 0", out);
}

void test_InStopwatchModeCheckIfStopwatchIsWorking(void)
{
	int ticks = convertToTicks(0, 0, 2, 9);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:1 0:0:2 9", out);
}

void test_TimeIsRunningEvenIfWeAreInStopwatchMode(void)
{
	char stopwatchOutput[OUT_SIZE];
	char clockOutput[OUT_SIZE];
		
	int time = convertToTicks(12, 15, 30, 1);
	int additional = convertToTicks(0, 0, 20, 0);

	// At 12:15:30 1, switch to stopwatch
	int i;
	for (i = 0; i < time; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	// Switch to stopwatch mode
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

	// Start stopwatch
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	for (i = 0; i < additional; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	// Stop
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

	output(&watch, stopwatchOutput);
	
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
	
	output(&watch, clockOutput);
	
	TEST_ASSERT_EQUAL_STRING("Mode:1 0:0:20 0", stopwatchOutput);
	TEST_ASSERT_EQUAL_STRING("Mode:0 12:15:50 1", clockOutput);
}

void test_StartingStopwatchAndSwitchingViewDoesNotStopStopwatch(void)
{
	// Switch to stopwatch mode
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	int ticks = convertToTicks(0, 12, 20, 9);

	int i;
	for (i = 0; i < ticks - 1; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);	
	EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

	output(&watch, out);
	TEST_ASSERT_EQUAL_STRING("Mode:1 0:12:20 9", out);
}

void test_EntersInSetClockMode(void)
{
	int ticks = convertToTicks(0, 20, 0, 0);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    
	EWatch_Dispatch(&watch, EW_SET_CLOCK_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:3 2:20:0 0", out);
}
