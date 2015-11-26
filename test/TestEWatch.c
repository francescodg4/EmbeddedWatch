#include "unity.h"
#include "../EWatch.h"
#include "utility.h"
#include <stdio.h>

#define OUT_SIZE 255

static EWatch watch;
static char out[OUT_SIZE];

static void output2(EWatch *w, enum EWatchMode mode, char *out);
static void output(EWatch *w, char *out)
{
	output2(w, CLOCK_MODE, out);
	/* int mode = EWatch_GetMode(w);
	int hours = EWatch_GetHours(w);
	int minutes = EWatch_GetMinutes(w);
	int seconds = EWatch_GetSeconds(w);
	int tenths = EWatch_GetTenths(w);

	snprintf(out, OUT_SIZE, "Mode:%d %d:%d:%d %d", mode, hours, minutes, seconds, tenths);
	*/
}

static void output2(EWatch *w, enum EWatchMode mode, char *out)
{
	int mode_ = EWatch_GetMode(w);
	int hours = EWatch_GetHours(w);
	int minutes = EWatch_GetMinutes(w);
	int seconds = EWatch_GetSeconds(w);
	int tenths = EWatch_GetTenths(w);
	enum AlarmState alarmState = EWatch_GetAlarmState(w);

	const char *alarmStates[] = {"off", "on", "expired"};
	const char *alarmSS = alarmStates[alarmState];

	switch (mode) {
	case CLOCK_MODE:
	case STOPWATCH_MODE:
	case TIMESET_MODE:
		snprintf(out, OUT_SIZE,
			 "Mode:%d %d:%d:%d %d", mode_, hours, minutes, seconds, tenths);
		break;
	case ALARM_MODE:
		snprintf(out, OUT_SIZE, 
			 "Mode:%d %d:%d:%d %d %s",
			 mode, hours, minutes, seconds, tenths, alarmSS); 
		break;

	default:
		break;
	}
}

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
	int i, ticks;
	ticks = convertToTenths(hours, minutes, seconds, tenths);

	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
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

// --------------- Stopwatch tests --------------- //
void test_StopwatchModeIntialStateAllZero(void)
{
	int i;
	for (i = 0; i < 800; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:0 0", out);
}

void test_StartStopwatchAndUpdateOutput(void)
{
	int ticks = convertToTicks(0, 0, 2, 9);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:2 9", out);
}

void test_CanResetStopwatchOnlyWhenStopped(void)
{
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	waitFor(0, 20, 12, 0);
	
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Ignore BUTTON_M
 
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Stop
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Reset
	
	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:0 0", out);
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
	
	TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:20 0", stopwatchOutput);
	TEST_ASSERT_EQUAL_STRING("Mode:0 12:15:50 1", clockOutput);
}

void test_StartingStopwatchAndSwitchingViewDoesNotStopStopwatch(void)
{
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG); 
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Start Stopwatch

	waitFor(0, 12, 20, 9);
	
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG); // Switch to Clock mode	

	waitFor(0, 0, 10, 0);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG); // Switch back to stopwatch

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:2 0:12:30 9", out);
}

// --------------- Timeset mode --------------- //
void test_TimesetModeInitializedWithCurrentTime(void)
{
	waitFor(0, 20, 2, 0);

	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:3 2:20:0 0", out);
}

void test_SetClockWhenSwitchViewBackToClockMode(void)
{
	int time = convertToTicks(2, 4, 11, 7);	
	EWatchClock_Set(&watch.clock, time);
	
	// Add 1 hour
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	
	// Add 30 minutes
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	int i;
	for (i = 0; i < 30; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
	
	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:0 3:34:0 0", out);
}

void test_SwitchToTimesetModeAndDecrementHoursAndMinutes(void)
{	
	EWatchClock_Set(&watch.clock, convertToTenths(12, 34, 0, 1));	
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	       
	// Decrement 1 h
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	
        // Switch to minutes mode and decrement
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);	
	int i;
	for (i = 0; i < 30; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	
	// Switch back to clock mode
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:0 11:4:0 0", out);
}

void test_ReturningToHoursAfterMinutesChangedIsAllowed(void)
{
	EWatchClock_Set(&watch.clock, convertToTenths(11, 23, 58, 8));

	// Switch to timeset mode
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	       
	// Increment 2 h
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	
        // Switch to minutes mode and decrement
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);	
	int i;
	for (i = 0; i < 3; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	
	// Return to hours
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	
	// Switch back to clock mode
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
	
	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:0 12:20:0 0", out);
}
	
void test_ClockTicksDoesNotAffectTimeset(void)
{
	EWatchClock_Set(&watch.clock, convertToTenths(12, 30, 34, 2));

	// Switch to timeset mode
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	
	// Clock is ticking...
	waitFor(0, 0, 25, 8);

	// Hours + 1
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	// Switch to minutes mode
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	int i;
	// Minute + 30
	for (i = 0; i < 30; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	// Switch back to clock
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:0 14:0:0 0", out);
}
	
void test_SwitchToTimesetFromStopwatchMode(void)
{
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	waitFor(0, 12, 33, 8);
	
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	
	output(&watch, out);

	TEST_ASSERT_EQUAL_STRING("Mode:3 0:12:0 0", out);	
}

void test_AlwaysCopyTheCurrentValueOfTimeWhenInTimesetMode(void)
{
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	// Switch to stopwatch mode
 	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
  	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Start stopwatch

	waitFor(12, 22, 31, 5);

 	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Stop stopwatch
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	output(&watch, out);
	
	TEST_ASSERT_EQUAL_STRING("Mode:3 12:22:0 0", out);
}

// --------------- Alarm mode --------------- //

void test_SwitchToAlarmModeShowsCurrentAlarm(void)
{
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	
	output2(&watch, ALARM_MODE, out);

	TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 off", out);
}

void test_FourAlarmSetSignalsSetTheAlarm(void)
{
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set alarm

	output2(&watch, ALARM_MODE, out);

	TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 on", out);
}

void test_EnteringInSetAlarmModeAndSetHoursAndSeconds(void)
{
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours

	// Add 2 hours
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	// Decrement by 2 minutes
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

	output2(&watch, ALARM_MODE, out);

	TEST_ASSERT_EQUAL_STRING("Mode:1 13:58:0 0 off", out);
}
