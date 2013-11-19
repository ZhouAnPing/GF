/* The following are standard include modules */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VMS
#include "ssp_cimlib:talkutl.h"
#include "ssp_cimlib:cimdefs.h"
#include "ssp_cimlib:parsutl.h"
#include "ssp_cimlib:trcutil.h"
#else
#include "talkutl.h"
#include "cimdefs.h"
#include "parsutl.h"
#include "trcutil.h"
#endif
#include "prisrv.h"
TLK_dmqRec *dmqRec;

#define LOGFILE "stderr"
#define TIMEOUT 30

void setTarget();
void setQueue();
void ShutDownSrv();
int main (int argc, char **argv)
{

    char SrvCmd[_SIZ_MSGBUFF]={0};
    char ClientMsg[_SIZ_MSGBUFF]={0}; 
    char ErrorMsg[_SIZ_GENFIELD]={0};
    int  Status=-1;
    int i=0;

    if ( TRC_Open( "TstLog", "stderr", 9) != _RET_SUCCESS)
    {
        fprintf( stderr, "%s: Test program error: Could not open log file [%s]. Quit.\n", 
           TRC_GetTime(),"stderr");
        TRC_Shutdown();
        exit(1);
    }

    /**** Initialize Alarm Events ****/
    if ( TRC_InitAlarm ( LOGFILE, "TESTPROG", ShutDownSrv)!= _RET_SUCCESS )
    {
        TRC_Send( LOGFILE,  _TRC_LVL_INFO, 
                  "%s:FATAL ERROR: Could not Init alarm - program exiting.\n",
                  TRC_GetTime() ); 
        TRC_Shutdown();
        exit(1);
    }
    TRC_Send( LOGFILE, _TRC_LVL_WARN, 
          "%s: Logfile and Alarm Init completed. Setting Queues...\n", TRC_GetTime());

    setQueue();

    do
    {

          TRC_Send( LOGFILE,  _TRC_LVL_INFO, "%s: Enter CmdText:\n",
                    TRC_GetTime());
          scanf( "%[^\n]s", SrvCmd);
          fflush(stdin);
          getchar();
          TRC_Send( LOGFILE,  _TRC_LVL_INFO, "Ok.\n");

          if( strcmp(SrvCmd, "SHUTDOWN") == 0)
          {
              TRC_Shutdown();
              TLK_DMQ_Shutdown();
              break;
          }
          if( strcmp(SrvCmd, "QUEUE") == 0)
          {
              setTarget();
                  continue;
          }

          strcpy(message.buffer, SrvCmd);
          message.msgLen=strlen(SrvCmd);
          message.msgPtr=0;
          TLK_DMQ_ClientPut(2);
          TLK_DMQ_ClientGet(2);
          if(TRC_ChkAlarm())
          TRC_Send( LOGFILE,  _TRC_LVL_INFO, "%s:Reply\n%s\n",
                    TRC_GetTime(), message.buffer);
          else
          TRC_Send( LOGFILE,  _TRC_LVL_INFO, "%s:Error occurred\n", 
                    TRC_GetTime());
    }while (1);
    ShutDownSrv();
    return 0;
}
/************************** End of PRIMain *********************************************/ 
void setQueue()
{
     int Status=0;
    Status = TLK_DMQ_SetQueue(  1
                  , "temp"
                  , PSYM_ATTACH_TEMPORARY
                  , LOGFILE
                  , _TRC_LVL_WARN);
     if (Status == _RET_SUCCESS) 
     {
         TRC_Send( LOGFILE,  _TRC_LVL_STATE, 
               "%s: TEST PROGRAM successfully set queue to [TEMP.1]\n",
               TRC_GetTime());
     }
     else
     {
         TRC_Send( LOGFILE, _TRC_LVL_ERROR,
               "%s: TEST PROGRAM Error setting queue to [TEMP.1]\n",
               TRC_GetTime());
         TRC_Shutdown();
         TLK_DMQ_Shutdown();
         exit(1);
     }
}

void setTarget()
{
    int Status=0;
    char qName[30]={0};

    TRC_Send( LOGFILE, _TRC_LVL_ERROR,
              "%s: Enter Queue Name to set target:\n",
              TRC_GetTime());

    scanf( "%[^\n]s", qName);
    fflush(stdin);
    getchar();

    Status = TLK_DMQ_SetTarget( 2
                              , qName
                              , 1
                              , TIMEOUT
                              , LOGFILE
                              , _TRC_LVL_WARN);
     if (Status == _RET_SUCCESS) 
     {
         TRC_Send( LOGFILE,  _TRC_LVL_STATE, 
               "%s: TEST PROGRAM successfully set target to [%s.2]\n",
               TRC_GetTime(), qName);
     }
     else
     {
         TRC_Send( LOGFILE, _TRC_LVL_ERROR,
               "%s: TEST PROGRAM Error setting target to [%s.1]\n",
               TRC_GetTime(), qName);
         TRC_Shutdown();
         TLK_DMQ_Shutdown();
         exit(1);
     }
}
/***  END: setTarget ************************************************************************/
void ShutDownSrv()
{
        TRC_Shutdown();
        TLK_DMQ_Shutdown();
        fprintf(stderr, "\n%s: Shutting down server...\n", TRC_GetTime());
        fprintf( stderr, "\n%s: Exit.\n\n", TRC_GetTime());
        exit(0);
}

