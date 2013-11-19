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
/*#include "f3priloc.h"
 #include "f3prisrv.h"
 */
TLK_dmqRec *dmqRec;

#define LOGFILE "stderr"
#define TIMEOUT 30
#define C5_QUEUE_NAME "C5SYNC"
#define hTARGET    1
#define hREPLY     3
void setTarget();
void setQueue();
void ShutDownSrv();
int main(int argc, char **argv) {
  char SrvCmd[_SIZ_MSGBUFF] = { 0 };
  int Status = -1;
  int i = 0;

  if (TRC_Open("TstLog", "stderr", 9) != _RET_SUCCESS) {
    fprintf(stderr, "%s: Test program error: Could not open log file [%s]. Quit.\n", TRC_GetTime(), "stderr");
    TRC_Shutdown();
    exit(1);
  }

  /**** Initialize Alarm Events ****/
  if (TRC_InitAlarm(LOGFILE, "C2SYNCStartup", ShutDownSrv) != _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s:FATAL ERROR: Could not Init alarm - program exiting.\n", TRC_GetTime());
    TRC_Shutdown();
    exit(1);
  }
  TRC_Send(LOGFILE, _TRC_LVL_WARN, "%s: Logfile and Alarm Init completed. Setting Queues...\n", TRC_GetTime());

  setQueue();
  setTarget();

  TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s: Send ROMARIC startup command to C5SYNC\n", TRC_GetTime());
  PRS_DEL_InitCommand("ROMARIC_STARTUP",  "Command When ROMARIC Startup successfully");
//  strcpy(message.buffer, SrvCmd);
  //message.msgLen = strlen(SrvCmd);
  //message.msgPtr = 0;
  TRC_ClrAlarm();
  TLK_DMQ_ClientPut(hTARGET);
//  TLK_DMQ_ClientGet(hTARGET);
  if (TRC_ChkAlarm())
    TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s:Reply\n%s\n", TRC_GetTime(), message.buffer);
  else
    TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s:Error %d occurred: %s\n", TRC_GetTime(), TRC_GetAlarm(), TRC_ErrorMsg());

  ShutDownSrv();
  return 0;
}
/************************** End of PRIMain *********************************************/
void setQueue() {
  int Status = 0;
  Status = TLK_DMQ_SetQueue(hREPLY, "temp", PSYM_ATTACH_TEMPORARY, LOGFILE, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: ROMARIC STARTUP PROGRAM successfully set queue to [TEMP.%d]\n",
             TRC_GetTime(), hREPLY);
  } else {
    TRC_Send(LOGFILE, _TRC_LVL_ERROR, "%s: ROMARIC STARTUP PROGRAM Error setting queue to [TEMP.%d]\n", TRC_GetTime(),
             hREPLY);
    TRC_Shutdown();
    TLK_DMQ_Shutdown();
    exit(1);
  }
}

void setTarget() {
  int Status = 0;

  Status = TLK_DMQ_SetTarget(hTARGET, C5_QUEUE_NAME, hREPLY, TIMEOUT, LOGFILE, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: ROMARIC STARTUP PROGRAM successfully set target to [%s.%d]\n", TRC_GetTime(),
             C5_QUEUE_NAME, hTARGET);
  } else {
    TRC_Send(LOGFILE, _TRC_LVL_ERROR, "%s: ROMARIC STARTUP TEST PROGRAM Error setting target to [%s.%d]\n",
             TRC_GetTime(), C5_QUEUE_NAME, hTARGET);
    TRC_Shutdown();
    TLK_DMQ_Shutdown();
    exit(1);
  }
}
/***  END: setTarget ************************************************************************/
void ShutDownSrv() {
  TRC_Shutdown();
  TLK_DMQ_Shutdown();
  fprintf(stderr, "\n%s: Shutting down server...\n", TRC_GetTime());
  fprintf(stderr, "\n%s: Exit.\n\n", TRC_GetTime());
  exit(0);
}
