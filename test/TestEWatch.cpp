#include <catch2/catch_all.hpp>

#include <EWatch.h>
#include "utility.h"

#define OUT_SIZE 255

static EWatch watch;

static std::string output(EWatch *w)
{
	int mode = EWatch_GetMode(w);
	int hours = EWatch_GetHours(w);
	int minutes = EWatch_GetMinutes(w);
	int seconds = EWatch_GetSeconds(w);
	int tenths = EWatch_GetTenths(w);
	enum AlarmState alarmState = EWatch_GetAlarmState(w);
	
	const char *alarmStates[] = {"off", "on", "expired"};
	const char *alarmSS = alarmStates[alarmState];

	static char buffer [OUT_SIZE];

	switch (mode) {
	case ALARM_MODE:
		snprintf(buffer, OUT_SIZE,
			 "Mode:%d %d:%d:%d %d %s",
			  mode, hours, minutes, seconds, tenths, alarmSS);
		break;

	default:
		snprintf(buffer, OUT_SIZE,
			 "Mode:%d %d:%d:%d %d", mode, hours, minutes, seconds, tenths);
		break;
	}

	return std::string(buffer);
}

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
	int i, ticks;
	ticks = convertToTenths(hours, minutes, seconds, tenths);

	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
}

TEST_CASE("Intialization As Clock Mode", "[watch]")
{
	EWatch watch;
	EWatch_Init(&watch);

	REQUIRE(output(&watch) == "Mode:0 0:0:0 0");
}

TEST_CASE("Receiving Clock Tick Event")
{
	EWatch_Init(&watch);

	int ticks = convertToTicks(13, 23, 22, 0);
	int i;

	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

	REQUIRE(output(&watch) == "Mode:0 13:23:22 0");
}

// --------------- Stopwatch tests --------------- //
TEST_CASE("test_StopwatchModeIntialStateAllZero(void)")
{
	EWatch_Init(&watch);

	int i;
	for (i = 0; i < 800; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

	REQUIRE(output(&watch) == "Mode:2 0:0:0 0");
}

TEST_CASE("test_StartStopwatchAndUpdateOutput(void)")
{
	EWatch_Init(&watch);

	int ticks = convertToTicks(0, 0, 2, 9);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	int i;
	for (i = 0; i < ticks; i++)
		EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

	REQUIRE(output(&watch) == "Mode:2 0:0:2 9");
}

TEST_CASE("test_CanResetStopwatchOnlyWhenStopped(void)")
{
	EWatch_Init(&watch);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	waitFor(0, 20, 12, 0);
	
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Ignore BUTTON_M
 
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Stop
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Reset

	REQUIRE(output(&watch) == "Mode:2 0:0:0 0");
}

TEST_CASE("test_TimeIsRunningEvenIfWeAreInStopwatchMode(void)")
{
	EWatch_Init(&watch);

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
	REQUIRE(output(&watch) == "Mode:2 0:0:20 0");
	
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
	REQUIRE(output(&watch) == "Mode:0 12:15:50 1");
}

TEST_CASE("test_StartingStopwatchAndSwitchingViewDoesNotStopStopwatch(void)")
{
	EWatch_Init(&watch);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Start Stopwatch

	waitFor(0, 12, 20, 9);
	
	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG); // Switch to Clock mode

	waitFor(0, 0, 10, 0);

	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG); // Switch back to stopwatch

	REQUIRE(output(&watch) == "Mode:2 0:12:30 9");
}

// --------------- Timeset mode --------------- //
TEST_CASE("test_TimesetModeInitializedWithCurrentTime(void)")
{
	EWatch_Init(&watch);

	waitFor(0, 20, 2, 0);

	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	REQUIRE(output(&watch) == "Mode:3 2:20:0 0");
}

TEST_CASE("test_SetClockWhenSwitchViewBackToClockMode(void)")
{
	EWatch_Init(&watch);
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

	REQUIRE(output(&watch) == "Mode:0 3:34:0 0");
}

TEST_CASE("test_SwitchToTimesetModeAndDecrementHoursAndMinutes(void)")
{
	EWatch_Init(&watch);
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

	;
	
	REQUIRE(output(&watch) == "Mode:0 11:4:0 0");
}

TEST_CASE("test_ReturningToHoursAfterMinutesChangedIsAllowed(void)")
{
	EWatch_Init(&watch);
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
	
	;
	
	REQUIRE(output(&watch) == "Mode:0 12:20:0 0");
}
	
TEST_CASE("test_ClockTicksDoesNotAffectTimeset(void)")
{
	EWatch_Init(&watch);
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

	;

	REQUIRE(output(&watch) == "Mode:0 14:0:0 0");
}
	
TEST_CASE("test_SwitchToTimesetFromStopwatchMode(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	waitFor(0, 12, 33, 8);
	
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
	
	;

	REQUIRE(output(&watch) == "Mode:3 0:12:0 0");
}

TEST_CASE("test_AlwaysCopyTheCurrentValueOfTimeWhenInTimesetMode(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	// Switch to stopwatch mode
 	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
  	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Start stopwatch

	waitFor(12, 22, 31, 5);

 	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Stop stopwatch
	EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

	;
	
	REQUIRE(output(&watch) == "Mode:3 12:22:0 0");
}

// --------------- Alarm mode --------------- //

TEST_CASE("test_SwitchToAlarmModeShowsCurrentAlarm(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	
	;

	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 off");
}

TEST_CASE("test_FourAlarmSetSignalsSetTheAlarm(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set alarm

	;

	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 on");
}

TEST_CASE("test_EnteringInSetAlarmModeAndSetHoursAndSeconds(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours

	// Add 2 hours
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	// Decrement by 2 minutes
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

	;

	REQUIRE(output(&watch) == "Mode:1 13:58:0 0 off");
}

TEST_CASE("test_StartAlarmAtTime(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	
	// Add 1 hour
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
	
	// Add 30 minutes
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

	int i;
	for (i = 0; i < 30; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	REQUIRE(output(&watch) == "Mode:1 13:30:0 0 off");

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

	REQUIRE(output(&watch) == "Mode:1 13:30:0 0 on");
}

TEST_CASE("test_SwitchBetweenStates(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 off");
}

TEST_CASE("test_AlarmExpireWhenIsTimeFromAnyState(void)")
{
	EWatch_Init(&watch);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // In alarm mode

	setAlarmTo(&watch.alarm, 12, 0);

	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 on");

	EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
	
	waitFor(11, 59, 59, 9);

	EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);	
	
	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 expired");

	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Switch alarm off	

	REQUIRE(output(&watch) == "Mode:1 12:0:0 0 off");
}

#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message) \
    INFO(message);                                           \
    REQUIRE(expected == actual);

TEST_CASE("test_AlarmCanBeChangedAfterHasBeenTurnedOn(void)")
{
	EWatch_Init(&watch);

	enum AlarmState alarmState;



	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
	setAlarmTo(&watch.alarm, 13, 30);

	alarmState = EWatch_GetAlarmState(&watch);

	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, alarmState, "Expected ALARM_ON");

	// Set alarm to different time
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in set hour mode

	alarmState = EWatch_GetAlarmState(&watch);
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_OFF, alarmState, "Expected ALARM_OFF");
	
	// Set hours
	EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

	// Set minutes
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes mode
	int i;
	for (i = 0; i < 30; i++)
		EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
	
	// Set new alarm
	EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

	alarmState = EWatch_GetAlarmState(&watch);
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, alarmState, "Expected ALARM_ON");

	REQUIRE(14 == EWatchAlarm_GetHours(&watch.alarm));
	REQUIRE(0 == EWatchAlarm_GetMinutes(&watch.alarm));

	waitFor(14, 0, 0, 0);

	alarmState = EWatch_GetAlarmState(&watch);
	TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, alarmState, "Expected ALARM_EXPIRED");
}

