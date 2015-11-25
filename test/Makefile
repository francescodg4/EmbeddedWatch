CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -Lunity -lunity

# # --------------- EWatch
# testEWatch: TARGET := testEWatch
# testEWatch: SRC := ../EWatch.c ../EWatchClock.c ../EWatchControl.c ../EWatchStopwatch.c
# testEWatch: TEST := TestEWatch.c

# # Generate runner file
# TEST_RUNNER = $(TEST:%.c=%_Runner.c)

# # Generate obj file
# OBJS = $(SRC:%.c=%.o)
# TEST_OBJS = $(TEST:%.c=%.o) $(TEST_RUNNER:%.c=%.o)

# $(TEST_RUNNER): $(TEST)
# 	ruby unity/auto/generate_test_runner.rb $< $@

# $(TEST_OBJS) utility.o: CFLAGS += -Iunity

# $(TARGET): $(TEST_OBJS) $(OBJS) utility.o
# 	$(CC) -o $(TARGET) $^ $(LDFLAGS)

# # --------------- EWatchStopwatch
# STOPWATCH_TARGET = testStopwatch
# STOPWATCH_SRC = ../EWatchStopwatch.c ../EWatchClock.c
# STOPWATCH_TEST = TestEWatchStopwatch.c

# # Generate runner file
# STOPWATCH_TEST_RUNNER = $(STOPWATCH_TEST:%.c=%_Runner.c)

# # Generate obj file
# STOPWATCH_OBJS = $(STOPWATCH_SRC:%.c=%.o)
# STOPWATCH_TEST_OBJS = $(STOPWATCH_TEST:%.c=%.o) $(STOPWATCH_TEST_RUNNER:%.c=%.o)

# $(STOPWATCH_TEST_RUNNER): $(STOPWATCH_TEST)
# 	ruby unity/auto/generate_test_runner.rb $< $@

# $(STOPWATCH_TEST_OBJS) utility.o: CFLAGS += -Iunity

# $(STOPWATCH_TARGET): $(STOPWATCH_TEST_OBJS) $(STOPWATCH_OBJS) utility.o
# 	$(CC) -o $(STOPWATCH_TARGET) $^ $(LDFLAGS)

# # --------------- EWatchAlarm
# ALARM_TARGET = testAlarm
# ALARM_SRC = ../EWatchAlarm.c ../EWatchClock.c
# ALARM_TEST = TestEWatchAlarm.c

# # Generate runner file
# ALARM_TEST_RUNNER = $(ALARM_TEST:%.c=%_Runner.c)

# # Generate obj file
# ALARM_OBJS = $(ALARM_SRC:%.c=%.o)
# ALARM_TEST_OBJS = $(ALARM_TEST:%.c=%.o) $(ALARM_TEST_RUNNER:%.c=%.o)

# $(ALARM_TEST_RUNNER): $(ALARM_TEST)
# 	ruby unity/auto/generate_test_runner.rb $< $@

# $(ALARM_TEST_OBJS) utility.o: CFLAGS += -Iunity

# $(ALARM_TARGET): $(ALARM_TEST_OBJS) $(ALARM_OBJS) utility.o
# 	$(CC) -o $(ALARM_TARGET) $^ $(LDFLAGS)

# --------------- EWatchClock
CLOCK_TARGET = testClock
CLOCK_SRC = ../EWatchClock.c ../ClockCounter.c
CLOCK_TEST = TestEWatchClock.c

# Generate runner file
CLOCK_TEST_RUNNER = $(CLOCK_TEST:%.c=%_Runner.c)

# Generate obj file
CLOCK_OBJS = $(CLOCK_SRC:%.c=%.o)
CLOCK_TEST_OBJS = $(CLOCK_TEST:%.c=%.o) $(CLOCK_TEST_RUNNER:%.c=%.o)

$(CLOCK_TEST_RUNNER): $(CLOCK_TEST)
	ruby unity/auto/generate_test_runner.rb $< $@

$(CLOCK_TEST_OBJS) utility.o: CFLAGS += -Iunity

$(CLOCK_TARGET): $(CLOCK_TEST_OBJS) $(CLOCK_OBJS) utility.o
	$(CC) -o $(CLOCK_TARGET) $^ $(LDFLAGS)

# --------------- EWatchTimeset
TARGET = testTimeset
SRC = ../EWatchTimeset.c ../EWatchClock.c
TEST = TestTimeset.c

# Generate runner file
TEST_RUNNER = $(TEST:%.c=%_Runner.c)

# Generate obj file
OBJS = $(SRC:%.c=%.o)
TEST_OBJS = $(TEST:%.c=%.o) $(TEST_RUNNER:%.c=%.o)

$(TEST_RUNNER): $(TEST)
	ruby unity/auto/generate_test_runner.rb $< $@

$(TEST_OBJS) utility.o: CFLAGS += -Iunity

$(TARGET): $(TEST_OBJS) $(OBJS) utility.o
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

PHONY: clean

clean:
	rm -f ../*.o
	rm -f *.o
	rm -f test*
	rm -f *_Runner.c
