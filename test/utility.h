#ifndef UTILITY_H
#define UTILITY_H

#include "../EWatchClock.h"

int convertToTicks(int hours, int minutes, int seconds, int tenths);

void checkTime(
	int expectedHours,
	int expectedMinutes,
	int expectedSeconds,
	int expectedTenths,
	EWatchClock *w);

#endif /* UTILITY_H */
