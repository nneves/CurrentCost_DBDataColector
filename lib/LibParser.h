//---------------------------------------------------------------------------------------------
// Generic Parser class to extract Commands/Data from Serial Port Communication
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date (A0): 10-Dec-2011
// Objective: Create a generic Class to parse data from Serial Port in commands+data format for
//            an easy control/test/debug/interface with external hardware connected to an MBED
//            microcontroller. 
//            Initial idea was to control a remote Step Motor interface driver (Pololu A4983)
//            via USB Serial Port commands, but latter the author also found that it could also
//            be used as an GPS NEMA data parser.
//---------------------------------------------------------------------------------------------
// Generic Serial Port Command Parser:
//---------------------------------------------------------------------------------------------
// DEMOCMD: "$CMD1=D0,D1,D2,D3,D4,D5\r\n"
//
// '$' -> StartChr
// '=' -> CmdSepChr
// ',' -> DataSepChr
// '\n' -> EndChr
// '\r' -> IgnoreListChr[0]
//
// "CMD1" -> Command
// ["D0","D1","D2","D3","D4","D5"] -> Data
//---------------------------------------------------------------------------------------------
#ifndef LibParserH
#define LibParserH
//---------------------------------------------------------------------------------------------
#include <vector>
#include <string>
//using std::string;
//using std::vector;
using namespace std;
//---------------------------------------------------------------------------------------------
// PARSER error codes: SendToParser() -> Error [-10,0], Sucess [1,10]
//---------------------------------------------------------------------------------------------
const int PARSER_ERROR_IGNOREDATA = -3;
const int PARSER_ERROR_IGNOREENDCHAR = -2;
const int PARSER_ERROR_NOSTARTCHAR = -1;
const int PARSER_ERROR = 0;
const int PARSER_STARTCHAR = 1;
const int PARSER_ENDCHAR = 2;
const int PARSER_CMDSEPCHAR = 3;
const int PARSER_DATASEPCHAR = 4;
const int PARSER_IGNORECHAR = 5;
const int PARSER_BUFFERDATA = 6;
//---------------------------------------------------------------------------------------------
// PARSEREVAL error codes: ParserEval() -> Error [-20,-11], Sucess [11,20]
//---------------------------------------------------------------------------------------------
const int PARSEREVAL_ERROR = -11;
const int PARSEREVAL_INCOMPLETE_SEQ = -12;
const int PARSEREVAL_CMDSEPCHAR_NOTFOUND = -13;

const int PARSEREVAL_COMPLETED = 11;
//---------------------------------------------------------------------------------------------
class TLibParser
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  string Buffer;        // Internal Buffer
  string parserbuffer;  // Buffer to Parse Start/End Cmd strings
  vector<string> vDataStrings; // Vector container for the Data Element Strings
  
  string StartCmd;      // Starting Char
  string EndCmd;        // Ending Char
  char Alt01EndChar;     // Alternative Ending Char
  char Alt02EndChar;     // Alternative Ending Char
 
  bool StartFlag;        // Found Starting Char
  bool EndFlag;          // Found Starting Char
      
  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TLibParser();
  ~TLibParser();
       
  void ConfigParser(string iStartCmd, string iEndCmd);
  void ConfigDefaultParser();
  int  SendToParser(char *pdata);
  void ShowBufferData();
  void ResetParser();  
  void SetAlternativeEndChar01(char iEndChar);
  void SetAlternativeEndChar02(char iEndChar);  
  vector<string>* GetParserEvalData();
  string GetElementData(string itag, string ftag);
};
//---------------------------------------------------------------------------------------------
#endif