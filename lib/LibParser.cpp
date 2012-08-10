//---------------------------------------------------------------------------------------------
#include <stdio.h>
//---------------------------------------------------------------------------------------------
#include "LibParser.h"
//---------------------------------------------------------------------------------------------
// Defines - remove comments to enable special mode
//---------------------------------------------------------------------------------------------
//#define internaldebug // print debug messages
//#define minimal_internaldebug
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------------------------
TLibParser::TLibParser()
{    
  #ifdef internaldebug
    printf("Calling TLibParser Contructor\r\n");     
  #endif     
  
  ResetParser();
  ConfigDefaultParser();
}
//---------------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------------
TLibParser::~TLibParser()
{ 
  #ifdef internaldebug
    printf("Calling TLibParser Destructor\r\n");    
  #endif     
}
//---------------------------------------------------------------------------------------------
// ConfigParser - Initialize Parser Internal Vars
void TLibParser::ConfigParser(string iStartCmd, string iEndCmd)
{
  StartCmd = iStartCmd;
  EndCmd = iEndCmd;
    
  Alt01EndChar = '\r';
  Alt02EndChar = '\n';
  
  #ifdef internaldebug
    printf("-> TLibParser::ConfigParser: %s|%s\r\n", StartCmd.c_str(), EndCmd.c_str());
  #endif       
}
//---------------------------------------------------------------------------------------------
// Config Default Parser 
void TLibParser::ConfigDefaultParser()
{
  #ifdef internaldebug
    printf("-> TLibParser::ConfigDefaultParser\r\n");
  #endif       

  ConfigParser("<customcmd>", "</customcmd>");
}
//---------------------------------------------------------------------------------------------
// Send Data to Parser internal Buffer
int TLibParser::SendToParser(char *pdata)
{     
  // Initial state, no Start/End Cmd found yet
  if(StartFlag == false && EndFlag == false)
  {
    // verify if the previous received parserbuffer+string(*pdata) if part or equals to StartCmd        
    if((parserbuffer+*pdata).compare(0,parserbuffer.length()+1,StartCmd,0,parserbuffer.length()+1) == 0)
    {
      parserbuffer += *pdata;      

      #ifdef internaldebug        
        if(StartFlag == true)
          printf("-> TLibParser::SendToParser: Found complete StartCmd [%s]\r\n",parserbuffer.c_str());
        else
          printf("-> TLibParser::SendToParser: Found partial StartCmd [%s]\r\n",parserbuffer.c_str());          
      #endif

      if(parserbuffer == StartCmd)
      {
        StartFlag = true;
        parserbuffer.clear();
      }
     
      if(StartFlag == true)
        return PARSER_STARTCHAR;
      else
        return PARSER_ERROR_NOSTARTCHAR;          
    }
    else
    {
      StartFlag = false;
      parserbuffer.clear();
      #ifdef internaldebug        
        printf("[-]%c\r\n",*pdata);
      #endif   
      
      return PARSER_ERROR_NOSTARTCHAR;          
    }
  }  
  else if(StartFlag == true && EndFlag == false)
  {
    // verify if the previous received parserbuffer+string(*pdata) if part or equals to StartCmd
    if((parserbuffer+*pdata).compare(0,parserbuffer.length()+1,EndCmd,0,parserbuffer.length()+1) == 0)
    {
      parserbuffer += *pdata;

      #ifdef internaldebug        
        if(EndFlag == true)
          printf("-> TLibParser::SendToParser: Found complete EndCmd [%s]\r\n",parserbuffer.c_str());
        else
          printf("-> TLibParser::SendToParser: Found partial EndCmd [%s]\r\n",parserbuffer.c_str());          
      #endif  

      if(parserbuffer == EndCmd)
      {
        EndFlag = true;
        parserbuffer.clear();
      }
    
      if(EndFlag == true)
        return PARSER_ENDCHAR;     
      else
        return PARSER_ERROR_IGNOREENDCHAR; // need to change this!!! not an error, adding data to buffer
    }
    else
    {      
      // copies *pdata to internal Buffer
      Buffer += parserbuffer;
      Buffer += *pdata;

      EndFlag = false;
      parserbuffer.clear();
      #ifdef internaldebug        
        printf("[--]%c\r\n",*pdata);
      #endif   
      
      // verify if it's an history data message, if so trash all data
      // will not require this info, database already contains enough data
      string hist("<hist>");
      if(Buffer.find(hist)!=string::npos)
      {
        // found <hist> - trash data - reset
        printf("Found <hist> tag: %s\r\n", Buffer.c_str());
        ResetParser();
        return PARSER_ERROR_IGNOREDATA;
      }

      return PARSER_ERROR_IGNOREENDCHAR;
    }  
  }
  
  // Found Start and End Cmd
  if(StartFlag == true && EndFlag == true)
  {     
      #ifdef minimal_internaldebug
        printf("Found Cmd: [%s]\r\n", Buffer.c_str());
      #endif

      #ifdef internaldebug
        printf("Found Cmd: [%s]", Buffer.c_str());
      #endif             
      // ResetParser() called after data is fetched
      //StartFlag = false;
      //EndFlag = false;
      //parserbuffer.clear();
      //Buffer.clear();
      return PARSEREVAL_COMPLETED;
  }

  return PARSER_BUFFERDATA;
}
//---------------------------------------------------------------------------------------------
// Show Buffer data
void TLibParser::ShowBufferData()
{
  #ifdef internaldebug
    printf("-> TLibParser::ShowBufferData: %s\r\n",Buffer.c_str());
  #endif       
}
//---------------------------------------------------------------------------------------------
// Reset Parser Vars
void TLibParser::ResetParser()
{
  #ifdef internaldebug
    printf("-> TLibParser::ResetParser\r\n\r\n");
  #endif       
  
  Buffer.clear();  
  parserbuffer.clear();
  
  StartFlag = false;
  EndFlag = false;
}
//---------------------------------------------------------------------------------------------
// alternative EndChar 01 - necessary for diferent GPS end string chars
void TLibParser::SetAlternativeEndChar01(char iEndChar)
{
  Alt01EndChar = iEndChar;
  #ifdef internaldebug
    printf("-> TLibParser::SetAlternativeEndChar01: %d\r\n", Alt01EndChar);
  #endif  
}
//---------------------------------------------------------------------------------------------
// alternative EndChar 01 - necessary for diferent GPS end string chars
void TLibParser::SetAlternativeEndChar02(char iEndChar)
{
  Alt02EndChar = iEndChar;
  #ifdef internaldebug
    printf("-> TLibParser::SetAlternativeEndChar01: %d\r\n", Alt02EndChar);
  #endif    
}
//---------------------------------------------------------------------------------------------
// returns vector pointer with parsed data strings
vector<string>* TLibParser::GetParserEvalData()
{
  return &vDataStrings;
}
//---------------------------------------------------------------------------------------------
string TLibParser::GetElementData(string itag, string ftag)
{
  string result;
  int i,f;

  i = Buffer.find(itag);
  f = Buffer.find(ftag);

  if(i < 0 || f < 0)
    return string("ERROR");

  result = Buffer.substr(i+itag.length(), f-(i+itag.length()));
  return result;
}
//---------------------------------------------------------------------------------------------