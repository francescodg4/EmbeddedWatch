CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -Lunity -lunity
TARGET = test

all: test $(STOPWATCH_TARGET)

test: EWatch.o TestEWatch.o TestEWatch_Runner.o EWatchControl.o EWatchClock.o
	$(CC) -o $(TARGET) $^ -Lunity -lunity
	./$@

TestEWatch_Runner.o: TestEWatch_Runner.c
	$(CC) -c -o $@ -Iunity $<

TestEWatch_Runner.c: TestEWatch.c
	ruby unity/auto/generate_test_runner.rb $< $@

TestEWatch.o: TestEWatch.c
	$(CC) -c $^ -Iunity

EWatch.o: ../EWatch.c
	$(CC) -c $^

EWatchControl.o: ../EWatchControl.c
	$(CC) -c $^

EWatchClock.o: ../EWatchClock.c
	$(CC) -c $^

# --------------- EWatchStopwatch
STOPWATCH_TARGET = testStopwatch
STOPWATCH_SRC = ../EWatchStopwatch.c ../EWatchClock.c
STOPWATCH_TEST = TestEWatchStopwatch.c

# Generate runner file
STOPWATCH_TEST_RUNNER = $(STOPWATCH_TEST:%.c=%_Runner.c)

# Generate obj file
STOPWATCH_OBJS = $(STOPWATCH_SRC:%.c=%.o)
STOPWATCH_TEST_OBJS = $(STOPWATCH_TEST:%.c=%.o) $(STOPWATCH_TEST_RUNNER:%.c=%.o)

$(STOPWATCH_TEST_RUNNER): $(STOPWATCH_TEST)
	ruby unity/auto/generate_test_runner.rb $< $@

$(STOPWATCH_TEST_OBJS) utility.o: CFLAGS += -Iunity

$(STOPWATCH_TARGET): $(STOPWATCH_TEST_OBJS) $(STOPWATCH_OBJS) utility.o
	$(CC) -o $(STOPWATCH_TARGET) $^ $(LDFLAGS)

# --------------- EWatchAlarm
ALARM_TARGET = testAlarm
ALARM_SRC = ../EWatchAlarm.c ../EWatchClock.c
ALARM_TEST = TestEWatchAlarm.c

# Generate runner file
ALARM_TEST_RUNNER = $(ALARM_TEST:%.c=%_Runner.c)

# Generate obj file
ALARM_OBJS = $(ALARM_SRC:%.c=%.o)
ALARM_TEST_OBJS = $(ALARM_TEST:%.c=%.o) $(ALARM_TEST_RUNNER:%.c=%.o)

$(ALARM_TEST_RUNNER): $(ALARM_TEST)
	ruby unity/auto/generate_test_runner.rb $< $@

$(ALARM_TEST_OBJS) utility.o: CFLAGS += -Iunity

$(ALARM_TARGET): $(ALARM_TEST_OBJS) $(ALARM_OBJS) utility.o
	$(CC) -o $(ALARM_TARGET) $^ $(LDFLAGS)

PHONY: clean

clean:
	rm -f ../*.o
	rm -f *.o
	rm -f $(TARGET)
	rm -f *_Runner.c
