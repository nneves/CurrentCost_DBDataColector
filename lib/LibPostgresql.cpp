//---------------------------------------------------------------------------------------------
#include <string>
using std::string;
#include <cstdlib>
//---------------------------------------------------------------------------------------------
#include "LibPostgresql.h"
//---------------------------------------------------------------------------------------------
// Defines - remove comments to enable special mode
//---------------------------------------------------------------------------------------------
//#define internaldebug // print debug messages
//#define minimal_internaldebug
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------------------------
TLibPostgresql::TLibPostgresql()
{
  #ifdef internaldebug
    printf("Calling TLibPostgresql Contructor\r\n");
  #endif

  cache_impulse = 0;

  conn = PQconnectdb("dbname=currentcostdb host=localhost user=ccuser password=ccpass");
  if(PQstatus(conn) == CONNECTION_BAD)
  {
    #ifdef internaldebug
      printf("We were unable to connect to the database");
    #endif
    flag_connected = false;
  }
  else
  {
    flag_connected = true;
  }
}
//---------------------------------------------------------------------------------------------
// destructor
//---------------------------------------------------------------------------------------------
TLibPostgresql::~TLibPostgresql()
{
  #ifdef internaldebug
    printf("Calling TLibPostgresql Destructor\r\n");
  #endif

  PQclear(res);
  PQfinish(conn);
}
//---------------------------------------------------------------------------------------------
int TLibPostgresql::InsertData(const char *sensor, const char *id, const char *time, const char *tmpr, 
	  const char *watts, const char *imp)
{
  int rec_count = 0;
  unsigned int impulse;

  if(!flag_connected)
    return -1;

  // convert imp to integer
  impulse = atoi(imp);

  string sql;
  if((string)watts != string("ERROR"))
  {
    sql += "INSERT INTO sensordata(sensor, sensortime, sensortemp, watts, impulse) ";
    //printf("%s\r\n", sql.c_str());
    sql += "VALUES("+
    	    (string)sensor+", "+
    	    "'"+(string)time+"', "+
    	    (string)tmpr+", "+
    	    (string)watts+", 0)";
    //printf("%s\r\n", sql.c_str());
  }
  else if((string)imp != string("ERROR"))
  {
    //prevent from adding lower impulse data - will mess with reports
    //if(impulse > 0 && impulse >= cache_impulse)
    if(true)
    {
      #ifdef minimal_internaldebug
        printf("Found impulse %d\r\n", impulse);
      #endif
        cache_impulse = impulse;

      sql += "INSERT INTO sensordata(sensor, sensortime, sensortemp, watts, impulse) ";
      //printf("%s\r\n", sql.c_str());
      sql += "VALUES("+
  	     (string)sensor+", "+
  	      "'"+(string)time+"', "+
  	     (string)tmpr+", 0, "+
  	     (string)imp+")";
      //printf("%s\r\n", sql.c_str());
    }
  }
  res = PQexec(conn, sql.c_str() );
  rec_count = PQntuples(res);

  return rec_count;
}
//---------------------------------------------------------------------------------------------
