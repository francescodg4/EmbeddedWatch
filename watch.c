#include <stdio.h>

int getTenths(int time)
{
  return time % 10;
}

int getSeconds(int time)
{
  return (time / 10) % 60;
}

int getMinutes(int time)
{
  return (time / 600) % 60;
}

int getHours(int time)
{
  return (time / 36000) % 24;
}

int main()
{
  int count = 864000 - 1;
  // count ++;

  int input = count;
  int hours = getHours(input);
  int minutes = getMinutes(input);
  int seconds = getSeconds(input);
  int tenths = getTenths(input);
    
    
  printf("%d:%d:%d %d\n", hours, minutes, seconds, tenths);
  
  return 0;
}
