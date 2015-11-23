CC = gcc
CFLAGS = -g
TARGET = test

test: EWatch.o TestEWatch.o TestEWatch_Runner.o EWatchControl.o
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

PHONY: clean

clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f *_Runner.c
