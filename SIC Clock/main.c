#include <stdio.h>

int main(void) {
  int timer = 48300; //this will be received from TIMER
  int hour = 0;
  int minute = 0;
  char mode = 0;
  hour = (timer/3600);
  minute = (timer-(3600*hour))/60;
  
  printf("%02d%02d\n",hour,minute);
  return 0;
}

