/*************************************************************************/
/*                                                                       */
/*   PRIMain.c:                                    Date:  Nov 2013  */
/*   ~~~~~~~~~~~~~                                   Author: Zhou An-Ping*/
/*                                                                       */
/*   This file contains the main program and top level routines for the   */
/*   PRI context server.                                                 */
/*                                                                       */
/*************************************************************************/

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*...CIMLIB include files...*/
#ifdef WIN32
#include "talkutl.h"
#include "parsutl.h"
#else
#include "ssp_cimlib:talkutl.h"
#include "ssp_cimlib:parsutl.h"
#endif

/*...MHServ include files...*/
#include "prisrv.h"

/*: IDENTIFIER :*********************************************************/
static char ServerId[][62] = { "$(%)@IdEnt:(&)C5SYNCsrv                     (@)Ver$ 1'0\" (#)$",
    "${&}Rel$ 1'0\"            (@)$", __DATE__, __TIME__, "(#)$" };

int transnet_down;

void ShutDownSrv();
void IDent(int);
void StartUp(int);
void server_shutdown(char *);

int main(int argc, char **argv) {

  char SrvCmd[_SIZ_GENFIELD];
  int Status;
  char ClientMsg[_SIZ_MSGBUFF];
  char ErrorMsg[_SIZ_GENFIELD];
  int Verbosity = -1, retval = 0, total = 0;

  transnet_down = 0;
  ErrorMsg[0] = '\0';

  /**** Cmd line Arguments ****/
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [verbosity|shutdown] \n\t%s\n\t%s\n", argv[0], "Verbosity 0-9",
            "shutdown to shutdown server.");
    exit(-1);
  }
  if (!strcmp(argv[1], "shutdown")) {
    server_shutdown(argv[0]);
    exit(0);
  }
  /*...  Only comes here if all ok  ...*/
  sscanf(argv[1], "%d", &Verbosity);
  if (Verbosity < 0 || Verbosity > 10)
    Verbosity = VERBOSITY;

  StartUp(0); /* print identifier to stderr */

  /**** Open Log  File ****/
  if (TRC_OpenEx(LOG_ALIAS, LOG_NAME, Verbosity, _TRC_NEXT_DAY, _LOG_2MB_LIMIT) != _RET_SUCCESS) {
    fprintf(stderr, "%s: C5SYNCsrv Error: Could not open log file [%s]. Quit.\n", TRC_GetTime(), LOG_ALIAS);
    TRC_Shutdown();
    exit(1);
  }

  /****** Now that the log file is open, log the startup of the server ******/
  StartUp(1);

  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Logfile Init completed. Setting Alarms.%s\n", "");
  /**** Initialize Alarm Events ****/
  if (TRC_InitAlarm(LOG_ALIAS, "C5SYNCsrv", ShutDownSrv) != _RET_SUCCESS) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: C5SYNCsrv Error: Error Initializing Alarm[%s]. Quit.\n", TRC_GetTime(),
             C5SYNCSRV_NAME);
    TRC_Shutdown();
    exit(1);
  }

  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Alarm Init completed. Attaching to the C5SYNCsrv main Queue.%s\n", "");

  /**** Attach to the C5SYNCSRV main Queue ****/

  if (TLK_DMQ_SetQueue(hC5SyncsrvPrimary, C5SYNCSRV_NAME, PSYM_ATTACH_BY_NAME, LOG_ALIAS, _TRC_LVL_WARN) != _RET_SUCCESS) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: C5SYNCsrv Error: Error Setting up queue [%s.%d].Quit.\n", TRC_GetTime(),
             C5SYNCSRV_NAME, hC5SyncsrvPrimary);
    TRC_Shutdown();
    exit(1);
  }

  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Alarm Init completed. Attaching to the C5SYNCsrv Reply Queue.%s\n", "");
  /**** Attach to Temporary Queue for Synchronous Server Transactions ****/
  if (TLK_DMQ_SetQueue(hC5SyncsrvReply, "temp", PSYM_ATTACH_TEMPORARY, LOG_ALIAS, _TRC_LVL_WARN) != _RET_SUCCESS) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: C5SYNCsrv Error: Error Setting up reply queue [%s.%d].Quit.\n",
             TRC_GetTime(), "temp", hC5SyncsrvReply);
    TRC_Shutdown();
    exit(1);
  }

  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Finish Attaching to C5SYNCsrv Reply Queue. Attaching to the MCSInt Queue.%s\n", "");
  Status = TLK_DMQ_SetTarget(hMCSIntServer, MCSINT_NAME, hC5SyncsrvReply, C5MCSSYNCSRV_TIMEOUT, LOG_ALIAS, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    Status = TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: MCSInt server send queue [%s.%d] setup successfully.\n",
                      TRC_GetTime(), MCSINT_NAME, hMCSIntServer);
  } else {
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: C5SYNCsrv Error: Error Setting up MCSInt server send queue[%s.%d].Quit.\n",
             TRC_GetTime(), MCSINT_NAME, hMCSIntServer);
    TRC_Shutdown();
    exit(1);
  }

  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Finish attaching to the MCSInt Queue. Attaching to the Transnet Queue.%s\n", "");
  Status = TLK_DMQ_SetTarget(hTransNetServer, TRANSNET_NAME, hC5SyncsrvReply, C5MCSSYNCSRV_TIMEOUT, LOG_ALIAS, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    Status = TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: TransNet server send queue [%s.%d] setup successfully.\n",
                      TRC_GetTime(), TRANSNET_NAME, hTransNetServer);
  } else {
    TRC_Send(LOG_ALIAS, _TRC_LVL_ERROR, "%s: C5SYNCsrv Error: Error Setting up TransNet server send queue[%s.%d].Quit.\n",
             TRC_GetTime(), TRANSNET_NAME, hTransNetServer);
    TRC_Shutdown();
    exit(1);
  }

  /*
   * Configure TransNet and begin to process.
   */
  __TRC_C5MCSSYNCSRV(_TRC_LVL_INFO, "%s: Finish attaching to the Transnet Queue. Init Transnet Parameter and init Global Section.%s\n", "");

  Status = MH_TransNet_Init(0);
  if (Status != _RET_SUCCESS) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Warning MHsrv unable to finish \"STARTUP\" sequence with PRI TransNet.\n",
             TRC_GetTime());
  } else {
    TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: MHsrv established \"STARTUP\" sequence with PRI TransNet successfully.\n",
             TRC_GetTime());
  }


  TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: Waiting for SrvCmds\n", TRC_GetTime());

  /**** Main loop:  Check for messages, process if required ****/
  do {
    TRC_Send(LOG_ALIAS, _TRC_LVL_INFO, "%s: Waiting for next SrvCmd\n", TRC_GetTime());

    /***
     ***   Retrieve the Client Message
     ***/
    Status = TLK_DMQ_ServerGet(hC5SyncsrvPrimary, _TLK_TYPDEL, ClientMsg);
    if (Status != _RET_SUCCESS) {
      //   printf("Main loop TLK_DMQ_ServerGet error.\n");
      continue; /* Skip to begining of loop */
    }
    SrvCmd[0] = '\0';

    /***
     ***   Get the Command Id
     ***/
    Status = PRS_VFI_GetToken(_PRS_VFI_CMDID, _SIZ_GENFIELD, SrvCmd);
    if (Status == _RET_SUCCESS) {
      TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: Processing [%s]\n", TRC_GetTime(), SrvCmd);
      if (!strcmp(SrvCmd, "SHUTDOWN")) {
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n", TRC_GetTime(), SrvCmd);
        break;
      }
      PRIDispatch(SrvCmd);
    } else {
      TRC_ClrAlarm();
      message.msgType = _TLK_TYPDEL;
      message.msgPtr = 0;
      PRS_DEL_SetDelim('|');
      PRS_DEL_GetNext(SrvCmd, _SIZ_GENFIELD);
      PRS_DEL_GetNext(SrvCmd, _SIZ_GENFIELD);

      if (TRC_ChkAlarm()) {
        TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: Processing (NON-VFEI) [%s]\n", TRC_GetTime(), SrvCmd);
        PRIDispatch_NVFEI(SrvCmd);
      }
    }
    /***
     *** Make call to execute failed setlocations
     ***/
    if (FAILLOTstart)
      CallExecFailedLots();

    /*  To reduce no. of retry of location updaing to Promis. Changed on 18092003 */
    /*  18092003: Retry once only. Release the list immediately.                  */
    if (FAILLOTstart)
      FreeFailedSetLocationsMem();
    /*  end add ***************************************************/

  } while (1 == 1);
  ShutDownSrv();
  return 0;
}

void ShutDownSrv() {

  if (FAILLOTstart)
    FreeFailedSetLocationsMem();
  TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "\n%s: Shutting down server...\n", TRC_GetTime());
  IDent(1);
  TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "\n%s: Exit.\n\n", TRC_GetTime());
  TRC_Shutdown();
  TLK_DMQ_Shutdown();
  fprintf(stderr, "\n%s: Shutting down server...\n", TRC_GetTime());
  IDent(0);
  fprintf(stderr, "\n%s: Exit.\n\n", TRC_GetTime());
  exit(0);
}
/************************** End of PRIMain ******************************/

/***  BEGIN: Add Identifying routine ************************************/

/************************************************************************
 **
 ** Function   : void IDent( int direction )
 **               function to identify the server executing.
 ** Parameters : int direction
 **                             0 - stderr
 **                             1 - current log file
 ** Returns    : void nothing
 **
 ***********************************************************************/
void IDent(int direction) {
  if (direction == 1)
    TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: %s\n%s: %s %s %s %s\n", TRC_GetTime(), ServerId[0], TRC_GetTime(),
             ServerId[1], ServerId[2], ServerId[3], ServerId[4]);
  else
    fprintf(stderr, "%s: %s\n%s: %s %s %s %s\n", TRC_GetTime(), ServerId[0], TRC_GetTime(), ServerId[1], ServerId[2],
            ServerId[3], ServerId[4]);
}
/***  END: Identifying routine ******************************************/

void StartUp(int direction) {
  if (direction == 1) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "\n%s: Starting up server...\n", TRC_GetTime());
    IDent(1);
    TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, "%s: Running.\n\n", TRC_GetTime());
  } else {
    fprintf(stderr, "%s: Starting up server...\n", TRC_GetTime());
    IDent(0);
    fprintf(stderr, "%s: Running.\n\n", TRC_GetTime());
  }
}

void server_shutdown(char *server) {
  //IF HAVE TIME TO ENHANCE - not in request
  /* 1. Connect to primary q
   2. Clear messages
   3. Put 'SHUTDOWN' on queue
   CMD/A="SHUTDOWN" MID/A="PRISRV" MTY/A="E" TID/U4=1 CMD_TYPE/A=""
   4. Exit
   */
  fprintf(stderr, "Sending shutdown to\n%s.\n", server);

  /*processing here */

  fprintf(stderr, "Shutdown sent to\n%s.\n", server);
  return;
}
