#ifndef UTILITY_H
#define UTILITY_H

#include "../ClockCounter.h"
#include "../EWatchAlarm.h"

int convertToTicks(int hours, int minutes, int seconds, int tenths);
void setAlarmTo(EWatchAlarm *a, int hours, int minutes);

void checkTime(
	int expectedHours,
	int expectedMinutes,
	int expectedSeconds,
	int expectedTenths,
	ClockCounter *c);

#endif /* UTILITY_H */
