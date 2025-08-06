#include <catch2/catch_all.hpp>

#include <EWatch.h>
#include "utility.h"

#define OUT_SIZE 255

static EWatch watch;
static char out[OUT_SIZE];

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    REQUIRE(std::string(expected) == std::string(actual));

static void output(EWatch* w, char* out)
{
    int mode = EWatch_GetMode(w);
    int hours = EWatch_GetHours(w);
    int minutes = EWatch_GetMinutes(w);
    int seconds = EWatch_GetSeconds(w);
    int tenths = EWatch_GetTenths(w);
    enum AlarmState alarmState = EWatch_GetAlarmState(w);

    const char* alarmStates[] = { "off", "on", "expired" };
    const char* alarmSS = alarmStates[alarmState];

    switch (mode) {
    case ALARM_MODE:
        snprintf(out, OUT_SIZE, "Mode:%d %d:%d:%d %d %s", mode, hours, minutes, seconds, tenths, alarmSS);
        break;

    default:
        snprintf(out, OUT_SIZE, "Mode:%d %d:%d:%d %d", mode, hours, minutes, seconds, tenths);
        break;
    }
}

static void waitFor(int hours, int minutes, int seconds, int tenths)
{
    int i, ticks;
    ticks = convertToTenths(hours, minutes, seconds, tenths);

    for (i = 0; i < ticks; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }
}

static void setUp(void)
{
    EWatch_Init(&watch);
}

static void setAlarmTo(EWatchAlarm* a, int hours, int minutes)
{
    EWatchTimeset_Set(&a->expirationTime, 0);

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set hours

    int i;
    for (i = 0; i < hours; i++) {
        EWatchAlarm_Dispatch(a, AL_INC_SIG);
    }

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set minutes

    for (i = 0; i < minutes; i++) {
        EWatchAlarm_Dispatch(a, AL_INC_SIG);
    }

    EWatchAlarm_Dispatch(a, AL_ALARM_SET_SIG); // Set alarm
}

TEST_CASE("At intialization EWatch is in ClockMode")
{
    EWatch watch;
    EWatch_Init(&watch);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 0:0:0 0", out);
}

TEST_CASE("Receiving ClockTick events")
{
    setUp();

    int ticks = utils::convertToTicks(13, 23, 22, 0);
    int i;

    for (i = 0; i < ticks; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 13:23:22 0", out);
}

// --------------- Stopwatch tests --------------- //
TEST_CASE("StopwatchMode intial state all zero", "[stopwatch]")
{
    setUp();

    int i;
    for (i = 0; i < 800; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }

    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:0 0", out);
}

TEST_CASE("Start stopwatch and update output", "[stopwatch]")
{
    setUp();

    int ticks = utils::convertToTicks(0, 0, 2, 9);

    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    int i;
    for (i = 0; i < ticks; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:2 9", out);
}

TEST_CASE("Can reset stopwatch only when stopped", "[stopwatch]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    waitFor(0, 20, 12, 0);

    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Ignore BUTTON_M

    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG); // Stop
    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG); // Reset

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:0 0", out);
}

TEST_CASE("Time is running even if we are in StopwatchMode", "[stopwatch]")
{
    setUp();

    char stopwatchOutput[OUT_SIZE];
    char clockOutput[OUT_SIZE];

    int time = utils::convertToTicks(12, 15, 30, 1);
    int additional = utils::convertToTicks(0, 0, 20, 0);

    // At 12:15:30 1, switch to stopwatch
    int i;
    for (i = 0; i < time; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }

    // Switch to stopwatch mode
    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);

    // Start stopwatch
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    for (i = 0; i < additional; i++) {
        EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    }

    // Stop
    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

    output(&watch, stopwatchOutput);

    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    output(&watch, clockOutput);

    TEST_ASSERT_EQUAL_STRING("Mode:2 0:0:20 0", stopwatchOutput);
    TEST_ASSERT_EQUAL_STRING("Mode:0 12:15:50 1", clockOutput);
}

TEST_CASE("Stopwatch will continue to run even if the current view changes", "[stopwatch]")
{
    setUp();

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
TEST_CASE("TimesetModeInitializedWithCurrentTime", "[timeset]")
{
    setUp();

    waitFor(0, 20, 2, 0);

    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:3 2:20:0 0", out);
}

TEST_CASE("SetClockWhenSwitchViewBackToClockMode", "[timeset]")
{
    setUp();

    int time = utils::convertToTicks(2, 4, 11, 7);
    EWatchClock_Set(&watch.clock, time);

    // Add 1 hour
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    // Add 30 minutes
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
    int i;
    for (i = 0; i < 30; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    }

    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 3:34:0 0", out);
}

TEST_CASE("Switch to TimeSetMode and decrement hours and minutes", "[timeset]")
{
    setUp();

    EWatchClock_Set(&watch.clock, convertToTenths(12, 34, 0, 1));
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

    // Decrement 1 h
    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

    // Switch to minutes mode and decrement
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
    int i;
    for (i = 0; i < 30; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
    }

    // Switch back to clock mode
    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 11:4:0 0", out);
}

TEST_CASE("Returning to hours after minutes changed is allowed", "[timeset]")
{
    setUp();

    EWatchClock_Set(&watch.clock, convertToTenths(11, 23, 58, 8));

    // Switch to timeset mode
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

    // Increment 2 h
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    // Switch to minutes mode and decrement
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
    int i;
    for (i = 0; i < 3; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
    }

    // Return to hours
    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

    // Switch back to clock mode
    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 12:20:0 0", out);
}

TEST_CASE("Clock ticks do not change TimeSet", "[timeset]")
{
    setUp();

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
    for (i = 0; i < 30; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    }

    // Switch back to clock
    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:0 14:0:0 0", out);
}

TEST_CASE("Switch To TimeSet From StopwatchMode", "[timeset]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    waitFor(0, 12, 33, 8);

    EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:3 0:12:0 0", out);
}

TEST_CASE("Always copy the current value of time when in TimeSetMode", "[timeset]")
{
    setUp();

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

TEST_CASE("Switch to AlarmMode shows current alarm", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 off", out);
}

TEST_CASE("Confirm AlarmSet signal is requested after setting hours and minutes", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set alarm

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 on", out);
}

TEST_CASE("In AlarmMode set hours and minutes", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Enters in alarm mode (does nothing)

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // -> Enters in Setting -> Set hours

    // Add 2 hours
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    // Decrement by 2 minutes
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Set minutes

    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 13:58:0 0 off", out);
}

TEST_CASE("Start alarm at time", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    // Add 1 hour
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
    EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);

    // Add 30 minutes
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    int i;
    for (i = 0; i < 30; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
    }

    output(&watch, out);
    TEST_ASSERT_EQUAL_STRING("Mode:1 13:30:0 0 off", out);

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    output(&watch, out);
    TEST_ASSERT_EQUAL_STRING("Mode:1 13:30:0 0 on", out);
}

TEST_CASE("Switch between states", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
    EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 off", out);
}

TEST_CASE("Alarm starts when its time expires from any states", "[alarm]")
{
    setUp();

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // In alarm mode

    setAlarmTo(&watch.alarm, 12, 0);

    output(&watch, out);
    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 on", out);

    EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);

    waitFor(11, 59, 59, 9);

    EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 expired", out);

    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG); // Switch alarm off

    output(&watch, out);

    TEST_ASSERT_EQUAL_STRING("Mode:1 12:0:0 0 off", out);
}

TEST_CASE("Alarm can be changed even after it has been activated", "[alarm]")
{
    setUp();

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
    for (i = 0; i < 30; i++) {
        EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
    }

    // Set new alarm
    EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);

    alarmState = EWatch_GetAlarmState(&watch);
    TEST_ASSERT_EQUAL_MESSAGE(ALARM_ON, alarmState, "Expected ALARM_ON");

    TEST_ASSERT_EQUAL(14, EWatchAlarm_GetHours(&watch.alarm));
    TEST_ASSERT_EQUAL(0, EWatchAlarm_GetMinutes(&watch.alarm));

    waitFor(14, 0, 0, 0);

    alarmState = EWatch_GetAlarmState(&watch);
    TEST_ASSERT_EQUAL_MESSAGE(ALARM_EXPIRED, alarmState, "Expected ALARM_EXPIRED");
}
