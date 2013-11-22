/********************************************************************************************/
/*                                                                       ********************/
/*   pristartip.c                                    Date:  October 2000 ********************/
/*   ~~~~~~~~~~~~~                                                       ********************/
/*                                                                       ********************/
/*                                                                       ********************/
/********************************************************************************************/

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*...CIMLIB include files...*/
#ifdef WIN32
#include "cimdefs.h"
#include "ordarray.h"
#include "trcutil.h"
#include "cimtokens.h"
#include "parsutl.h"
#include "talkutl.h"
#else
#include "ssp_cimlib:cimdefs.h"
#include "ssp_cimlib:ordarray.h"
#include "ssp_cimlib:trcutil.h"
#include "ssp_cimlib:cimtokens.h"
#include "ssp_cimlib:parsutl.h"
#include "ssp_cimlib:talkutl.h"
#endif

/*...MHServ include files...*/
#include "prisrv.h"

int Sleep(int);
/**************************************************************************

 MH_TransNet_VFI_Startup:                           Date:  June 1997
 ~~~~~~~~~~~~~~~~~~~~~~~~                         Author:  Chun Wu
 Revised 1/20/98:  Mike Hochberg

 This function sends the first VFEI command to TransNet.
 REV: If CMD_ACK has a zero ECD, it is OK to startup Transnet.
 If CMD_ACK has a non-zero ECD, Transnet in up already.
 If no CMD_ACK, Transnet is not ready for a startup.

 Arguments:
 ~~~~~~~~~~

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 VFEI Message:
 ~~~~~~~~~~~~~
 Data Item      Type            Value
 ==============================================
 COMMAND_ID     ASCII           "MACH_CMD"
 MACHINE_ID     ASCII           "TRANSNET"|"HOST"
 MSG_TYPE       ASCII           "C"
 TID            UNSIGNED 4      <transaction_id>
 COMMAND_TYPE   ASCII           "STARTUP"

 Expected VFEI Reply:
 ~~~~~~~~~~~~~~~~~~~~
 Data Item      Type            Value
 =======================================================
 COMMAND_ID     ASCII           "MACH_CMD"
 MACHINE_ID     ASCII           "TRANSNET"|"HOST"
 MSG_TYPE       ASCII           "R"
 TID            UNSIGNED 4      <transaction_id>
 COMMAND_TYPE   ASCII           "STARTUP"
 ERROR_CODE     UNSIGNED 4      <error_code>
 ERROR_TEXT     ASCII           "error_text"

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 **************************************************************************/

int MH_TransNet_VFI_Startup(void) {
  char errTxt[_SIZ_GENFIELD];
  char cmd[_SIZ_GENFIELD];
  unsigned int errorcd;
  int errCode, status;
  char ErrorMsg[_SIZ_GENFIELD];

  /*
   * Server sends "STARTUP" VFEI command to PRI TransNet.
   */
  TRC_ClrAlarm();
  PRS_VFI_InitCommand(_TOK_MACHCMD, "TRANSNET");

  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "STARTUP");

  /*....................................................................*/
  /* Send command and get response                                     */

  message.msgType = _TLK_TYPVFEI;

  if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 1. Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }
  if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 2. Status = %d\t%s\n",
    //          TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }

  /*
   * Log failure to the logfile handle.
   */
  if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
    status = PRS_VFI_GetToken("ECD", _PRS_UINT, &errorcd);
    if (errorcd == 50101) {
      /***********************************************************************************
       *** Added this code to send a shutdown after transnet replies that the link is not
       *** down when we startup. After sending shut send start and go to wait.
       ***/
      TRC_ClrAlarm();
      PRS_VFI_InitCommand(_TOK_MACHCMD, "TRANSNET");

      if (TRC_ChkAlarm())
        PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "SHUTDOWN");
      message.msgType = _TLK_TYPVFEI;
      TLK_DMQ_ClientPut(hTransNetServer);
      TLK_DMQ_ClientGet(hTransNetServer);

      /***  End addition of shutdown send
       ***
       ***********************************************************************************/
      /***********************************************************************************
       *** Added this code to PAUSE 10 SECONDS AFTER sending a shutdown after transnet
       *** replies that the link is not
       *** down when we startup. After sending shut send start and go to wait.
       *** wu lei, david, tanit 01 aug 2001
       ***/
      /**** change to 60s because of MCS s/w upgrade ****/
#ifndef WIN32
      /*** Change to cater new server, original was Sleep(60); ***/
      Sleep(200);
#endif

      /***  End addition of pause
       ***
       ***********************************************************************************/

      TRC_ClrAlarm();
      PRS_VFI_InitCommand(_TOK_MACHCMD, "TRANSNET");

      if (TRC_ChkAlarm())
        PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "STARTUP");
      message.msgType = _TLK_TYPVFEI;
      if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
        // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 3. Status = %d\t%s\n",
        //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
        return (TRC_GetAlarm());
      }
      if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
        // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 4. Status = %d\t%s\n",
        //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
        return (TRC_GetAlarm());
      }

      /*
       * Log failure to the logfile handle.
       */
      if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
        status = PRS_VFI_GetToken("ECD", _PRS_UINT, &errorcd);
        if (errorcd != 1) {
          // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 5. Status = %d\t%s\n",
          //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
          return (TRC_GetAlarm());
        }
      }
    }
  }
  if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) {
    return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "CMD"));
  } else {
    if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0)
      return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_CMDWRONG, cmd, "CMD_ACK"));
  }

  if (PRS_VFI_GetToken("ETX", _SIZ_GENFIELD, errTxt) != _RET_SUCCESS) {
    return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "ETX"));
  }
  if (PRS_VFI_GetToken("ECD", _PRS_UINT, &errCode) != _RET_SUCCESS)
    return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "ECD"));
  //    if (errCode == 50101)
  if (errCode == 1) {
    TRC_ClrAlarm();
    PRIMethodShutDown(ErrorMsg);
    TRC_ClrAlarm();
    PRS_VFI_InitCommand(_TOK_MACHCMD, "TRANSNET");
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "STARTUP");

    message.msgType = _TLK_TYPVFEI;

    if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
      //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 6. Status = %d\t%s\n",
      //          TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      return (TRC_GetAlarm());
    }
    if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
      //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 7. Status = %d\t%s\n",
      //          TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      return (TRC_GetAlarm());
    }
    if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
      status = PRS_VFI_GetToken("ECD", _PRS_UINT, &errorcd);
      if (errorcd != 50101) {
        //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 8. Status = %d\t%s\n",
        //          TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
        return (TRC_GetAlarm());
      }
    }
    if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) {
      return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "CMD"));
    } else {
      if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0)
        return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_CMDWRONG, cmd, "CMD_ACK"));
    }

    if (PRS_VFI_GetToken("ETX", _SIZ_GENFIELD, errTxt) != _RET_SUCCESS) {
      return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "ETX"));
    }
    if (PRS_VFI_GetToken("ECD", _PRS_UINT, &errCode) != _RET_SUCCESS)
      return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_FLDMISSING, "ECD"));
  }

  if (errCode != 0)
    TransNet_Status = 0;
  if (errCode == 0)
    TransNet_Status = 1;
  return (TRC_SetAlarm("MH_TransNet_VFI_Startup", _RET_SUCCESS));
}

/******************** End of MH_TransNet_VFI_Startup *********************/

/**************************************************************************

 MH_TransNet_VFI_AlarmSetup:                        Date:  June 1997
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~                      Author:  Chun Wu

 This function request PRI TransNet to start its alarm.

 Arguments:
 ~~~~~~~~~~
 - Enable (type is unsigned short - read):
 This variable acts as an switch for enabling (1) or disabling (0)
 a particular alarm.

 - AlarmId (type is int - read):
 This is the identification of the alarm that we are interested
 at.  If Enable has a value of 0, then this value is ignored.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 VFEI Message:
 ~~~~~~~~~~~~~
 Data Item      Type            Value
 ==============================================
 COMMAND_ID     ASCII           "ALARM_SETUP"
 MACHINE_ID     ASCII           "TRANSNET"
 MSG_TYPE       ASCII           "C"
 TID            UNSIGNED 4      <transaction_id>
 ENABLE         UNSIGNED 1      0|1
 ALARM_ID       INTEGER 4       <alarm_id>

 Expected VFEI Reply:
 ~~~~~~~~~~~~~~~~~~~~
 Data Item      Type            Value
 =======================================================
 COMMAND_ID     ASCII           "CMD_ACK"
 MACHINE_ID     ASCII           "MHsrv"
 MSG_TYPE       ASCII           "R"
 TID            UNSIGNED 4      <transaction_id>
 ERROR_CODE     UNSIGNED 4      <error_code>
 ERROR_TEXT     ASCII           "error_text"

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 **************************************************************************/

int MH_TransNet_VFI_AlarmSetup(int Enable, int AlarmId) {
  char dataVal[_SIZ_MSGBUFF];
//    int Status;
  unsigned int errCode;
  int zero;

  TRC_ClrAlarm();

  zero = 0;
  /*......................................................................*/
  /* Sends command and get responce                                        */
  PRS_VFI_InitCommand("ALARM_SETUP", TRANSNET_NAME);

  sprintf(dataVal, "ENABLE/U1=%u", Enable);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken("ENABLE", _PRS_UINT1, &Enable);
  /*
   if (Enable >0)
   {
   sprintf(dataVal, "ALARM_ID/I4[1]=[%d]", AlarmId);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken ("ALARM_ID",_PRS_INT,&AlarmId);
   }
   else
   */
  if (TRC_ChkAlarm())
    PRS_VFI_PutArray("ALARM_ID", _PRS_INT, 1, &zero);

  /*
   * Assign out going message type and class
   */

  message.msgType = _TLK_TYPVFEI;

  if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
    return (TRC_GetAlarm());
    // TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //                 TRC_GetTime(),  _RET_TRANSNET_ALARM_SETUP_FAIL, _TRC_TRANSNET_ALARM_SETUP_FAIL);
    // old e.g.return (TRC_SetAlarm("MH_TransNet_VFI_AlarmSetup", _RET_TRANSNET_ALARM_SETUP_FAIL));
  }

  if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_ALARM_SETUP_FAIL, _TRC_TRANSNET_ALARM_SETUP_FAIL);
    return (TRC_GetAlarm());
  }

  if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_EVENT_SETUP_FAIL, _TRC_TRANSNET_EVENT_SETUP_FAIL);
    return (TRC_GetAlarm());
  }

  if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, dataVal) == _RET_SUCCESS) {
    if (PRS_VFI_GetToken("ECD", _PRS_UINT, &errCode) != _RET_SUCCESS)
      return (TRC_SetAlarm("MH_TransNet_VFI_AlarmSetup", _RET_FLDMISSING, "ECD"));
    if (errCode != 0)
      return (TRC_GetAlarm());
  } else {
    return (TRC_SetAlarm("MH_TransNet_VFI_AlarmSetup", _RET_FLDMISSING, "CMD"));
  }

  return (TRC_SetAlarm("MH_TransNet_VFI_AlarmSetup", _RET_SUCCESS));
}

/***************** End of MH_TransNet_VFI_AlarmSetup *********************/

/**************************************************************************

 MH_TransNet_VFI_EventSetup:                        Date:  June 1997
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~                      Author:  Chun Wu

 This function sends "EVENT_SETUP" to TransNet.

 Arguments:
 ~~~~~~~~~~
 - Enable (type is unsigned short - read):
 This variable acts as an switch for setting up a particular
 alarm (ie. 1) or all alarm (ie: 0).

 - EventId (type is char* - read):
 The event that the caller would like to have it setup.

 - VarId (type is *char[] - read):
 This array contains optional configuration values for setting
 up an event.

 - ArraySize (type is int - read):
 This value indicate how many elements are in the array.

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 VFEI Message:
 ~~~~~~~~~~~~~
 Data Item      Type            Value
 ==============================================
 COMMAND_ID     ASCII           "EVENT_SETUP"
 MACHINE_ID     ASCII           "TRANSNET"
 MSG_TYPE       ASCII           "C"
 TID            UNSIGNED 4      <transaction_id>
 ENABLE         UNSIGNED 1      <0|1>
 EVENT_ID       ASCII           "CARRIER_REMOVED" |
 "CARRIER_ENTERED" |
 "LOC_CHANGED" |
 "CAP_REACHED" |
 "CAP_RESET" |
 "STATUS_CHANGED"
 VAR_ID         ASCII[]         ["..."]

 Expected VFEI Reply:
 ~~~~~~~~~~~~~~~~~~~~
 Data Item      Type            Value
 =======================================================
 COMMAND_ID     ASCII           "CMD_ACK"
 MACHINE_ID     ASCII           "MHSRV"
 MSG_TYPE       ASCII           "R"
 TID            UNSIGNED 4      <transaction_id>
 ERROR_CODE     UNSIGNED 4      <error_code>
 ERROR_TEXT     ASCII           "error_text"

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 **************************************************************************/

int MH_TransNet_VFI_EventSetup(unsigned int Enable, char *EventId, char VarId[][_SIZ_GENFIELD], int ArraySize) {
  char dataVal[_SIZ_MSGBUFF];
//    int Status;
  unsigned int errCode;

  TRC_ClrAlarm();

  /*......................................................................*/
  /* Sends command and get responce                                        */
  PRS_VFI_InitCommand("EVENT_SETUP", TRANSNET_NAME);

  sprintf(dataVal, "ENABLE/U1=%u", Enable);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken("ENABLE", _PRS_UINT1, &Enable);

  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_EVENTID, _SIZ_EVENTID, EventId);
  if (TRC_ChkAlarm())
    PRS_VFI_PutArray(_TOK_VARID, _SIZ_GENFIELD, ArraySize, VarId);
  /*
   * Assign out going message type and class
   */

  message.msgType = _TLK_TYPVFEI;

  if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
    return (TRC_GetAlarm());
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_EVENT_SETUP_FAIL, _TRC_TRANSNET_EVENT_SETUP_FAIL);
    //     return (TRC_SetAlarm("MH_TransNet_VFI_EventSetup", _RET_TRANSNET_EVENT_SETUP_FAIL));
  }

  if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //          TRC_GetTime(), _RET_TRANSNET_EVENT_SETUP_FAIL, _TRC_TRANSNET_EVENT_SETUP_FAIL);
    return (TRC_GetAlarm());
  }

  if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //          TRC_GetTime(), _RET_TRANSNET_EVENT_SETUP_FAIL, _TRC_TRANSNET_EVENT_SETUP_FAIL);
    return (TRC_GetAlarm());
  }

  if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, dataVal) == _RET_SUCCESS) {
    if (PRS_VFI_GetToken("ECD", _PRS_UINT, &errCode) != _RET_SUCCESS)
      return (TRC_SetAlarm("MH_TransNet_VFI_EventSetup", _RET_FLDMISSING, "ECD"));
    if (errCode != 0)
      return (TRC_GetAlarm());
  } else {
    return (TRC_SetAlarm("MH_TransNet_VFI_EventSetup", _RET_FLDMISSING, "CMD"));
  }
  return (TRC_SetAlarm("MH_TransNet_VFI_EventSetup", _RET_SUCCESS));
}

/****************** End of MH_TransNet_VFI_EventSetup ********************/

/**************************************************************************

 MH_TransNet_Init:                                  Date:  June 1997
 ~~~~~~~~~~~~~~~~~                                Author:  Chun Wu
 Revised 1/20/98:  Mike Hochberg

 This function will establish the actual VFEI dialog between
 Material Handling Server(aka. MHsrv) and PRI TransNet system.

 1/98 -- Remove requirement for a Get_Host_Time.  Make it optional.

 Arguments:
 ~~~~~~~~~~

 Communication Scenario:
 ~~~~~~~~~~~~~~~~~~~~~~~
 Top half of the communication model can be initiated by either
 TransNet or MHsrv.  The bottom half can only be initiated by
 MHsrv.

 Server (TransNet)              Client (MHsrv)
 ================================================
 GET_HOST_TIME
 HOST_TIME
 STARTUP
 CMD_ACK
 ................................................
 ALARMSETUP
 CMD_ACK
 EVENTSETUP
 CMD_ACK
 SET_LOT_INFO_VARS
 CMD_ACK
 HOST_AUTHORIZE
 CMD_ACK

 Returns:
 ~~~~~~~~
 Type is int - coded value representing the completion status of the
 function.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 **************************************************************************/

int MH_TransNet_Init(unsigned int Tid) {
  char errTxt[_SIZ_GENFIELD];
  char machId[_SIZ_EQPTMID];
  char *EventId;
  char VarId[_MAX_DCVAR][_SIZ_GENFIELD];
  int AlarmId;
  int ArraySize;
  unsigned int errCode;
  unsigned int tid;
  unsigned short Evt_Enable;
  int Enable;

  TRC_ClrAlarm();

  /*
   * If the last known status of PRI TransNet is SERVER_ONLINE
   * and the request of "startup" is not from the PRI TransNet 
   * itself then there is no need to perform Startup sequence.
   */
  if ((TransNet_Status == 1) && (TransNet_Startup == 0))
    return (TRC_SetAlarm("PRI_TransNet_Init", _RET_SUCCESS));

  /*
   * Call MH_TransNet_VFI_Startup(), if MHsrv initiated the sequence
   */

  if (TransNet_Startup == 0) {
    if (MH_TransNet_VFI_Startup() != _RET_SUCCESS) {
      // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: 9. Status = %d\t%s\n",
      //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: %s\n", TRC_GetTime(),TRC_ErrorMsg());
      return (TRC_GetAlarm());
    } else {
      if (PRS_VFI_GetToken("TID", _PRS_UINT, &tid) != _RET_SUCCESS)
        return (TRC_SetAlarm("MH_TransNet_Init", _RET_FLDMISSING, "TID"));
    }
    //              Host_Authorize();
  } else {
    tid = Tid;
    /*
     * Send CMD_ACK reply back to TransNet
     */
    strcpy(errTxt, "");
    errCode = 0;
    strcpy(machId, TRANSNET_NAME);
    PRIReplyCmdack(machId);
  }

  Enable = 1;
  AlarmId = 0;

  if (MH_TransNet_VFI_AlarmSetup(Enable, AlarmId) != _RET_SUCCESS) {
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }

  Evt_Enable = 1;
  EventId = (char*) calloc(1, strlen("CARRIER_REMOVED") + 1);
  if (!EventId) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n", TRC_GetTime(), _RET_MHSRV_MEMORY_ALLOCATION_FAIL,
             _TRC_MHSRV_MEMORY_ALLOCATION_FAIL);
    return (TRC_SetAlarm("MH_TransNet_Init", _RET_MHSRV_MEMORY_ALLOCATION_FAIL));
  }
  strcpy(EventId, "CARRIER_REMOVED");
  strcpy(VarId[0], "CARRIERID");
  strcpy(VarId[1], "AMHSEQUIPID");
  strcpy(VarId[2], "PORT_ID");
  //      strcpy(VarId[3], "SECTION");
  strcpy(VarId[3], _TOK_TAGID);
  strcpy(VarId[4], _TOK_TAGDATA);

  //    ArraySize = 4;
  //    ArraySize = 3;
  ArraySize = 5;
  if (MH_TransNet_VFI_EventSetup(Evt_Enable, EventId, VarId, ArraySize) != _RET_SUCCESS) {
    //  TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //            TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }

  Evt_Enable = 1;
  strcpy(EventId, "LOC_CHANGED");
  strcpy(VarId[0], "CARRIERID");
  strcpy(VarId[1], "AMHSEQUIPID");
  strcpy(VarId[2], "CATEGORY");
  strcpy(VarId[3], "PRIMARYDEST");
  strcpy(VarId[4], "NUMBER");
  strcpy(VarId[5], _TOK_TAGID);
  strcpy(VarId[6], _TOK_TAGDATA);

  //    ArraySize = 5;
  ArraySize = 7;
  if (MH_TransNet_VFI_EventSetup(Evt_Enable, EventId, VarId, ArraySize) != _RET_SUCCESS) {
    // TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //           TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }

  Evt_Enable = 1;
  strcpy(EventId, "CARRIER_ENTERED");
  strcpy(VarId[0], "CARRIERID");
  strcpy(VarId[1], "AMHSEQUIPID");
  strcpy(VarId[2], "CATEGORY");
  strcpy(VarId[3], "NUMBER");
  strcpy(VarId[4], _TOK_TAGID);
  strcpy(VarId[5], _TOK_TAGDATA);

  //    ArraySize = 4;
  ArraySize = 6;

  if (MH_TransNet_VFI_EventSetup(Evt_Enable, EventId, VarId, ArraySize) != _RET_SUCCESS) {
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //          TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    return (TRC_GetAlarm());
  }

  Host_Authorize();
  List_DB();
  if (TRC_ChkAlarm()) {
    return (TRC_SetAlarm("MH_TransNet_Init", _RET_SUCCESS));
  } else
    return (TRC_GetAlarm());
}

/************************ End of MH_TransNet_Init ************************/
/************************ Start of List_DB function ************************/

int List_DB(void) {
  char vidArray[_MAX_VIDS][_SIZ_VID];
  char cmd[_SIZ_GENFIELD] = { 0 };
  int pgnumber = 0, pglength = 0, count = 0, tmppglength = 0, i = 0, smpcount = 0, vidArraySize = 0, tagidArraySize = 0,
      retval = 0, tagdataArraySize = 0, siz = 0, TokLocnamePos = 0;
  char tmpamhsequipid[_MAX_CARRIER][_SIZ_AMHSEQUIPID];
  char tmpcarrierid[_MAX_CARRIER][_SIZ_CARRIERID];
  char tmpcategory[_MAX_CARRIER][_SIZ_CATEGORY];
  char tmpnumber[_MAX_CARRIER][_SIZ_TMPNUMBER];
  char dummynumber[_MAX_CARRIER][_SIZ_DUMMYNUMBER];
  char s1[3], s2[3], s3[3], s4[3];
  //     char Amhsequipid[_SIZ_AMHSEQUIPID];
  //     char Carrierid[_SIZ_CARRIERID];
  //     char Category[_SIZ_CATEGORY];
  //     char Number[_SIZ_NUMBER];
  //     char ErrorMsg[_SIZ_GENFIELD];
  //     int ptpstatus;

  char PromUsrId[_SIZ_USERID] = { 0 };
  char PromPasswd[_SIZ_PASSWORD] = { 0 };
  char CarrierEntered[_SIZ_GENFIELD] = { 0 };
  //  char tmplocname[_MAX_CARRIER][_SIZ_GENFIELD];
  char tagid[_NUM_TAGS][_SIZ_LOCNAME];
  char tagdata[_MAX_CARRIER * _NUM_TAGS][_SIZ_LOCNAME];
  char tmpSMP[_MAX_CARRIER + 1] = { 0 };

  TRC_ClrAlarm();
  strcpy(vidArray[0], "AMHSEQUIPID");
  strcpy(vidArray[1], "CARRIERID");
  strcpy(vidArray[2], "CATEGORY");
  strcpy(vidArray[3], "NUMBER");
  strcpy(vidArray[4], _TOK_TAGID);
  strcpy(vidArray[5], _TOK_TAGDATA);
  vidArraySize = 6;
  siz = strlen(_TOK_LOCNAME);
  pgnumber = 0;
  pglength = 30;

  do {
    pgnumber++;
    //      printf("performing status query for list_db function \n");

    PRS_VFI_InitCommand("STATUS_QUERY", TRANSNET_NAME);
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "LIST_DB");
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_AMHSEQUIPID, _SIZ_AMHSEQUIPID, "ALL");
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_KEYATTRIBUTE, _SIZ_KEYATTRIBUTE, "ALL");
    if (TRC_ChkAlarm())
      PRS_VFI_PutArray(_TOK_VARID, _SIZ_VID, vidArraySize, vidArray);
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_PAGENUMBER, _PRS_UINT, &pgnumber);
    if (TRC_ChkAlarm())
      PRS_VFI_PutToken(_TOK_PAGELENGTH, _PRS_UINT, &pglength);

    message.msgType = _TLK_TYPVFEI;

    if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
      //TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
      //                TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      //  return (TRC_SetAlarm("DB List ", _RET_TRANSNET_INIT_FAIL));
      return (TRC_GetAlarm());
    }

    if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
      //TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
      //                TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      //  return (TRC_SetAlarm("DB List ", _RET_TRANSNET_INIT_FAIL));
      return (TRC_GetAlarm());
    }

    /*
     * Log failure to the logfile handle.
     */
    if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
      //TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
      //                TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
      //  return (TRC_SetAlarm("DB List ", _RET_TRANSNET_INIT_FAIL));
      return (TRC_GetAlarm());
    }

    if (PRS_VFI_GetToken("CMD", _SIZ_MSGBUFF, cmd) != _RET_SUCCESS) {
      return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "CMD"));
    } else {
      if (strcmp(cmd, "STATUS_LIST") != 0)
        return (TRC_SetAlarm("DB List ", _RET_CMDWRONG, cmd, "STATUS_LIST"));
      if (PRS_VFI_GetToken("PAGELENGTH", _PRS_UINT, &tmppglength) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "PAGELENGTH"));
      if (PRS_VFI_GetArray("AMHSEQUIPID", _MAX_CARRIER, _SIZ_AMHSEQUIPID, &count, tmpamhsequipid) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "AMHSEQUIPID"));
      if (PRS_VFI_GetArray("CARRIERID", _MAX_CARRIER, _SIZ_CARRIERID, &count, tmpcarrierid) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "CARRIERID"));
      if (PRS_VFI_GetArray("CATEGORY", _MAX_CARRIER, _SIZ_CATEGORY, &count, tmpcategory) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "CATEGORY"));
      if (PRS_VFI_GetArray("NUMBER", _MAX_CARRIER, _SIZ_DUMMYNUMBER, &count, dummynumber) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, "NUMBER"));
      for (i = 0; i < count; i++) {
        if (strlen(dummynumber[i]) > 7) {
          sscanf(dummynumber[i], "%3s%2s%1s%3s%1s%3s", s4, s1, s4, s2, s4, s3);
          sprintf(tmpnumber[i], "%s%s%s", s1, s2, s3);
        } else{
          sprintf(tmpnumber[i], "%s", dummynumber[i]);
        }
        sprintf ( CarrierEntered,"%s,%s",tmpamhsequipid[i], tmpnumber[i]);
        fprintf(stderr, "Carrier from MCSSIM is [%d]%s, Location %s\n", i,CarrierEntered,tmpcarrierid[i]);

        //update PROMIS Global Section
		size_t lotId_len = strlen(tmpcarrierid[i]);
		size_t  mcsLoc_len = strlen(CarrierEntered);  //_SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1;
		updateLotLocInPROMIS(tmpcarrierid[i], CarrierEntered, &lotId_len, &mcsLoc_len);

      }
      /***********************************************************************************
       *** Mods for location added here
       ***
       ***   Array tmpSMP to contain F or S to indicate F3 or SMP location
       ***   Use it later to filter lots to send
       ***/

      if (PRS_VFI_GetArray(_TOK_TAGID, _NUM_TAGS, _SIZ_LOCNAME, &tagidArraySize, tagid) != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_TAGID));

      /*locate which one is "PROMISLOC" in the list of TAG_IDs */
      for (TokLocnamePos = 0; (TokLocnamePos < tagidArraySize); ++TokLocnamePos)
        if (!strncmp(tagid[TokLocnamePos], _TOK_LOCNAME, siz))
          break;

      if (TokLocnamePos == tagidArraySize) {
        /*
         * Array of TAG_IDs does not contain a tag called "PROMISLOC" (_TOK_LOCNAME)
         * therefore signal error and return
         */
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_LOCNAME));
      }

      if (PRS_VFI_GetArray(_TOK_TAGDATA, _MAX_CARRIER * _NUM_TAGS, _SIZ_LOCNAME, &tagdataArraySize, tagdata)
          != _RET_SUCCESS)
        return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_LOCNAME));

      /* tagdata contains m*n items. m=tagidArraySize. TokLocnamePos = position of promisloc in array */
      smpcount = 0;
      for (i = 0; TokLocnamePos < tagdataArraySize; TokLocnamePos += tagidArraySize, ++i) {
        // changes for blank promis location to be skipped
        if (strlen(tagdata[TokLocnamePos]) == 0) {
          tmpSMP[i] = 'X';
          continue;  // skip current as it is blank location
        }
        tmpSMP[i] = 'F';
        if (IsSMPLoc(tagdata[TokLocnamePos])) {
          tmpSMP[i] = 'S';
          ++smpcount;
        }

        //     fprintf( stderr, "[%c][%s][%s][%d]\n", tmpSMP[i], tmpcarrierid[i]
        //                    , tagdata[TokLocnamePos], smpcount);
      }

      tmpSMP[i] = '\0';
      //       fprintf(stderr, "tmpSMP[%s]\n", tmpSMP);

      /***
       *** Mods for location added here
       ***
       ***********************************************************************************/



      //     if (TRC_ChkAlarm()) PRS_PTP_ChkStatus ();
      //     return (TRC_GetAlarm() );
    }
  } while (pglength == count); /* we still check count here as that is the original number of
   values got from the message */
  return (TRC_ChkAlarm());
}

/************************ End of List_DB function ************************/

/******************* Start of Host Authorisation function **********************/

int Host_Authorize(void) {
  char cmd[_SIZ_GENFIELD];
  int ecd, him;

  TRC_ClrAlarm();
  him = 1;

  PRS_VFI_InitCommand("MACH_CMD", TRANSNET_NAME);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "HOST_AUTHORIZE");
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken("HIM", _PRS_UINT, &him);

  message.msgType = _TLK_TYPVFEI;

//      printf("executing host_authorise function \n");
  if (TLK_DMQ_ClientPut(hTransNetServer) != _RET_SUCCESS) {
    return (TRC_GetAlarm());
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    //return (TRC_SetAlarm("HOST_AUTHORIZE ", _RET_TRANSNET_INIT_FAIL));
  }

  if (TLK_DMQ_ClientGet(hTransNetServer) != _RET_SUCCESS) {
    return (TRC_GetAlarm());
    //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
    //TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
  }

  /*
   * Log failure to the logfile handle.
   */
  if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
    PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd);
    if (ecd != 50101) {
      return (TRC_GetAlarm());
      //TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
      //TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
    }
  }

  if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS)
    return (TRC_SetAlarm("HOST_AUTHORIZE", _RET_FLDMISSING, "CMD"));

  if (strncmp(cmd, "CMD_ACK", strlen("STATUS_LIST")) != 0)
    return (TRC_SetAlarm("HOST_AUTHORIZE ", _RET_CMDWRONG, cmd, "CMD_ACK"));

  if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS)
    return (TRC_SetAlarm("HOST_AUTHORIZE", _RET_FLDMISSING, "ECD"));

  if (ecd != 0)
    return (TRC_SetAlarm("HOST_AUTHORIZE ", _RET_FAILURE, &ecd, "ECD NOT ZERO"));
  return (TRC_SetAlarm("HOST_AUTHORIZE", _RET_SUCCESS));
}

/*************************************************************************/
/********************* End of Host Authorize function *****************/

/*************************************************************************/
/********************* End of file MHsrv_TransNet_Init.c *****************/
/*************************************************************************/

