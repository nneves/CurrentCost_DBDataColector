//---------------------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>
//---------------------------------------------------------------------------------------------
#include "LibCommon.h"
//---------------------------------------------------------------------------------------------
/*
int getch(void)
{
  int ch;
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt); //store old settings 
  newt = oldt; // copy old settings to new settings 
  newt.c_lflag &= ~(ICANON | ECHO); // make one change to old settings in new settings
  tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply the new settings immediatly
  ch = getchar(); // standard getchar call
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reapply the old settings
  return ch; // return received char
  */
}
//---------------------------------------------------------------------------------------------