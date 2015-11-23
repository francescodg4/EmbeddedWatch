#include "EWatch.h"

void EWatch_Init(EWatch *this) 
{
	this->counter = 0;
}

void EWatch_Dispatch(EWatch *this, enum EWatchSignal sig)
{
	this->counter++;
}

int EWatch_GetHours(EWatch *this)
{
	return (this->counter / 36000) % 24;
}

int EWatch_GetMinutes(EWatch *this)
{
	return (this->counter / 600) % 60;
}


int EWatch_GetSeconds(EWatch *this)
{
	return (this->counter / 10) % 60;
}

int EWatch_GetTenths(EWatch *this)
{
	return this->counter % 10;
}

/* #include <stdio.h> */

/* int getTenths(int time) */
/* { */
/*   return time % 10; */
/* } */

/* int getSeconds(int time) */
/* { */
/*   return (time / 10) % 60; */
/* } */

/* int getMinutes(int time) */
/* { */
/*   return (time / 600) % 60; */
/* } */

/* int getHours(int time) */
/* { */
/*   return (time / 36000) % 24; */
/* } */

/* int main() */
/* { */
/*   int count = 864000 - 1; */
/*   // count ++; */

/*   int input = count; */
/*   int hours = getHours(input); */
/*   int minutes = getMinutes(input); */
/*   int seconds = getSeconds(input); */
/*   int tenths = getTenths(input); */
    
    
/*   printf("%d:%d:%d %d\n", hours, minutes, seconds, tenths); */
  
/*   return 0; */
/* } */
