#include "utility.h"

int convertToTicks(int hours, int minutes, int seconds, int tenths)
{
	int ticks = 0;

	ticks += hours * TENTHS_IN_HOUR;
	ticks += minutes * TENTHS_IN_MINUTE;
	ticks += seconds * TENTHS_IN_SECOND;
	ticks += tenths;

	return ticks;
}
