//-----------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// 
// Objective: Create a C application to parse Serial Port data 
//
// Hardware:
//
// Extra 
// 
// License:     You may use, distribute or change it as you wish. Please don't forget to 
//              include a reference to the original author :).
//-----------------------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
//-----------------------------------------------------------------------------------------
#include <string.h>
#include <string>
#include <vector>
using std::string;
using namespace std;
//-----------------------------------------------------------------------------------------
#include "../lib/LibParser.h"
//#include "../lib/LibCommon.h"
#include "../lib/LibPostgresql.h"
//---------------------------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------------------------
#define BAUDRATE1 B4800
#define BAUDRATE2 B38400
#define BAUDRATE3 B57600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
//-----------------------------------------------------------------------------------------
#define FALSE 0
#define TRUE 1
//-----------------------------------------------------------------------------------------
static volatile int exec_end = FALSE;
//-----------------------------------------------------------------------------------------
// Defines - remove comments to enable special mode
//---------------------------------------------------------------------------------------------
//#define internaldebug // prints debug messages 
#define minimal_internaldebug
#define localechoon // prints back received data
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
void cleanup(int sig)
{
  printf("\r\nCONTROL+C was caught... terminating in a friendly manner!\r\n");
  exec_end = TRUE;
  return;
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------------
int main() 
{ 
  //-------------------------------------------------------------------------------------------
  // variables for serial port communication
  //-------------------------------------------------------------------------------------------
  int fd,res;
  struct termios oldtio,newtio;  
  //-------------------------------------------------------------------------------------------
  char data[2];
  int result; 
  vector<string>* pvDataStrings;  
  //-------------------------------------------------------------------------------------------

  signal(SIGABRT, &cleanup);
  signal(SIGTERM, &cleanup);
  signal(SIGINT, &cleanup);
  signal(SIGKILL, &cleanup);

  //---------------------------------------------------------------------------------------
  // save current port settings
  tcgetattr(fd,&oldtio); 
  //---------------------------------------------------------------------------------------
  // open serial port
  fd = open(MODEMDEVICE, O_RDONLY | O_NOCTTY );
  if(fd<0) 
  {
    printf("Error: open serial port\r\n");
    perror(MODEMDEVICE);
    exit(-1); 
  }
  //---------------------------------------------------------------------------------------
  // setting serial port configurations
  //bzero(&newtio, sizeof(newtio));
  memset(&newtio, 0, sizeof(newtio));
  newtio.c_cflag = BAUDRATE3 | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;          // set input mode (non-canonical, no echo,...)
  newtio.c_cc[VTIME]    = 0;   // inter-character timer unused
  newtio.c_cc[VMIN]     = 1;   // blocking read until x chars received
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
  //---------------------------------------------------------------------------------------
  //write(fd, "SENDCMD", 7);
  //---------------------------------------------------------------------------------------

  TLibParser *pParser = NULL;
  pParser = new TLibParser();

  TLibPostgresql *pPostgresql = NULL;
  pPostgresql = new TLibPostgresql();

  #ifdef internaldebug
    printf("\r\nStarting CurrentCost EnviR Serial Data Parser application\r\n");
  #endif

  pParser->ConfigParser("<msg>","</msg>");
  pParser->SetAlternativeEndChar01('\r');
  pParser->SetAlternativeEndChar02('\n');

  while(exec_end==FALSE)
  {
    // loop for input
    res = read(fd,data,1);   // returns after 1 chars have been input (newtio.c_cc[VMIN]=1)
    data[1]=0;

    // check for errors - abort when receiving code 0x00
    if(data[0]=='#')
    {
      exec_end=TRUE;
      break;
    }

    //printf("%c",data[0]);
    //printf("[%x]",data[0]);

    result = pParser->SendToParser(&data[0]);
    #ifdef localechoon
      if(result > PARSER_ERROR)
        printf("%c",data[0]);
    #endif

    if(result == PARSEREVAL_COMPLETED)
    {
      string time = pParser->GetElementData("<time>", "</time>");
      string id = pParser->GetElementData("<id>", "</id>");
      string sensor = pParser->GetElementData("<sensor>", "</sensor>");
      string watts = pParser->GetElementData("<watts>", "</watts>");
      string imp = pParser->GetElementData("<imp>", "</imp>");
      string tmpr = pParser->GetElementData("<tmpr>", "</tmpr>");

      // Save Postgresql data
      pPostgresql->InsertData(sensor.c_str(),
                              id.c_str(),
                              time.c_str(),
                              tmpr.c_str(),
                              watts.c_str(),
                              imp.c_str());

      #ifdef minimal_internaldebug
        printf("TIME=%s,ID=%s,SENSOR=%s,TMPR=%s",
          time.c_str(),
          id.c_str(),
          sensor.c_str(),
          tmpr.c_str());
        //if(watts.find("ERROR") < 0)
        if(watts != "ERROR")
          printf(",WATTS=%s", watts.c_str());
        //if(imp.find("ERROR") < 0)
        if(imp != "ERROR")
          printf(",IMP=%s", imp.c_str());
        printf("\r\n");
      #endif

      pParser->ResetParser();
    }

    if(result == PARSER_BUFFERDATA)
    {
      exec_end=TRUE;
      break;
    }

/*
    if(result == PARSEREVAL_COMPLETED)
    {
      #ifdef internaldebug
        printf("\r\nCMD: %s\r\n",pParser->GetParserEvalCmd().c_str());
        pvDataStrings = pParser->GetParserEvalData();

        // print all vector->strings data
        for(vector<string>::const_iterator it = pvDataStrings->begin(); it != pvDataStrings->end(); ++it)
          printf("-> TLibParser::ParserEval Data: %s\r\n",it->c_str());

      #endif
    }
*/

    // TEST ONLY
    /*
    data = getch();

    if(data == '#')
      break; // exit

    #ifdef internaldebug
      printf("* %c\r\n",data);
    #endif

    result = pParser->SendToParser(&data);
    if(result == PARSER_ENDCHAR)
    {
      pParser->ShowBufferData();
    }
    */
  }

  delete pParser;
  pParser = NULL;

  delete pPostgresql;
  pPostgresql = NULL;

  //---------------------------------------------------------------------------------------
  // close serial port
  close(fd);
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // restore serial port old configurations
  tcsetattr(fd,TCSANOW,&oldtio);
  //---------------------------------------------------------------------------------------

  return 0;
}
//---------------------------------------------------------------------------------------------
