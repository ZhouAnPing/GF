/********************************************************************************************/
/*                                                                       ********************/
/*   mcsmethod.c:                                    Date:  July 2013
 ~~~~~~~~~~~~~                                     Author: Zhou An-Ping ********************/
/*                                                                       ********************/
/*                                                                       ********************/
/********************************************************************************************/

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
#include "mcsint.h"

/**
 * Get lot location from ROMARIC, then Toshiba, then Promis Global section
 */
int MCSMethodGetLotLoc(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg) {
  strcpy(Amhsequipid, "");
  strcpy(Location, "");
  MCSMethodGetLotLocFromROMARIC(Carrierid, Amhsequipid, Location, mcsstatus, ErrorMsg);
  DbgPrintf(stderr,"%s:[MCSMethodGetLotLoc]Location in ROMARIC is:%s\n", TRC_GetTime(), Location);
  if (strlen(Location) == 0 || strcmp(Location, "Transit") == 0) {
    MCSMethodGetLotLocFromToshiba(Carrierid, Amhsequipid, Location, mcsstatus, ErrorMsg);
    if (strlen(Location) == 0) {
      MCSMethodGetLocFromPromis(Carrierid, Amhsequipid, Location);
    }
  }
  return (TRC_GetAlarm());
}
/**
 * Get lot locaiton from Toshiba
 */
int MCSMethodGetLotLocFromToshiba(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg) {
  char temp[_SIZ_GENFIELD];
  TRC_ClrAlarm();
  if (TRC_ChkAlarm())
    PRS_DEL_InitCommand("MCS_GET_LOT_LOC", "F235MCSInt query");
  if (TRC_ChkAlarm())
    PRS_DEL_PutToken(Carrierid);

  /*.................................................................... */
  /* Send command and get response...                                   */
  if (TRC_ChkAlarm())
    TLK_DMQ_ClientPut(hToshibaServer);
  if (TRC_ChkAlarm())
    TLK_DMQ_ClientGet(hToshibaServer);

  /*.....................................................................*/
  /* Make sure that there was successful completion...                 */
  PRS_DEL_ChkStatus(_PRS_MCS_SUCMSG);
  DbgPrintf(stderr,"%s:****Receive message from TOSHIBA MCS*****\n%s\n", TRC_GetTime(), message.buffer);

  if (TRC_ChkAlarm()) {
    //*mcsstatus = 1;
    message.msgType = _TLK_TYPDEL;
    message.msgPtr = 0;
    PRS_DEL_SetDelim('|');
    strcpy(Amhsequipid, "");
    strcpy(Location, "");
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(Amhsequipid, _SIZ_AMHSEQUIPID);
    PRS_DEL_GetNext(Location, _SIZ_LOCATIONID);

  } else {
    //*mcsstatus = 0;
    strcpy(ErrorMsg, TRC_ErrorMsg());
    return (TRC_GetAlarm());
  }
  return (TRC_GetAlarm());
}
/**
 * Get lot Location from ROMARIC
 */
int MCSMethodGetLotLocFromROMARIC(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg) {
  char temp[_SIZ_GENFIELD];
  char *receivedMsg;
  char *sendMsg;
  // char receivedMsg[8000];
  int Status;
  TRC_ClrAlarm();
  if (TRC_ChkAlarm())
    PRS_DEL_InitCommand("MCS_GET_LOT_LOC", "F235MCSInt query");
  if (TRC_ChkAlarm())
    PRS_DEL_PutToken(Carrierid);

  /*.................................................................... */
  /* Send command and get response...                                   */

  Status = checkHSMSState();
  if (Status == 1) {
    strcpy(ErrorMsg, TRC_GetTime());
    strcat(ErrorMsg, "time out from ROMARIC MCS");
    //*mcsstatus = 0;
    return -1;
  }


   sendMsg = malloc(message.msgLen+1);
   strncpy(sendMsg, message.buffer, message.msgLen);
   receivedMsg = malloc(_SIZ_HSMSBUFF);
  if (!HSMS_SendAndWait(sendMsg, receivedMsg, _SIZ_HSMSBUFF)) {
    DbgPrintf(stderr,"%s:Receive Message:%s\n", TRC_GetTime(), receivedMsg);

    strcpy(ErrorMsg, TRC_GetTime());
    strcat(ErrorMsg, "Fail to get lot location from ROMARIC MCS");
    //*mcsstatus = 0;
    free(sendMsg);
    free(receivedMsg);
    return -1;
  }
  DbgPrintf(stderr,"%s:****Receive message from ROMARIC MCS*****\n%s\n", TRC_GetTime(), receivedMsg);
  strncpy(message.buffer, receivedMsg, strlen(receivedMsg));
  message.msgLen = strlen(receivedMsg);
  free(sendMsg);
  free(receivedMsg);

  /*.....................................................................*/
  /* Make sure that there was successful completion...                 */

  PRS_DEL_ChkStatus(_PRS_MCS_SUCMSG);


  if (TRC_ChkAlarm()) {
    message.msgType = _TLK_TYPDEL;
    message.msgPtr = 0;
    PRS_DEL_SetDelim('|');
    strcpy(Amhsequipid, "");
    strcpy(Location, "");
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(temp, _SIZ_GENFIELD);
    PRS_DEL_GetNext(Amhsequipid, _SIZ_AMHSEQUIPID);
    PRS_DEL_GetNext(Location, _SIZ_LOCATIONID);

  } else {
   // *mcsstatus = 1;
    strcpy(ErrorMsg, TRC_ErrorMsg());
    return (TRC_GetAlarm());
  }
  return (TRC_GetAlarm());
}

int MCSMethodGetLocFromPromis(char *Carrierid, char *Amhsequipid, char *mcsLocation) {
  struct FortranLocation fortranLoc;
  char temploc[] = "                 ";
  char *tmpEQPId;
  strcpy(Amhsequipid, "");
  strcpy(mcsLocation, "");
  size_t lotId_len = strlen(Carrierid);
  //get Lot previous equipment
  fortranLoc.length = _SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1;
  fortranLoc.location = temploc;
  getLotLocFromPromis(&fortranLoc, Carrierid, &lotId_len);
  DbgPrintf(stderr,"%s:[MCSMethodSendLotLoc]CarrierId|%s|Promis Location|%s|\n", TRC_GetTime(), Carrierid,temploc);

  if (strlen(temploc) == 0) {
    return TRC_SetAlarm("ClearLotLocation", _RET_GENERIC, "Lot is not exist in Promis global section.");
  }
  tmpEQPId = strtok(temploc, ",");
  if (tmpEQPId != (char*) NULL) {
    strcpy(Amhsequipid, tmpEQPId);
    strcpy(mcsLocation, &temploc[strlen(Amhsequipid) + 1]);
  }

  if (strlen(Amhsequipid) == 0 || strlen(mcsLocation) == 0) {
    return TRC_SetAlarm("ClearLotLocation", _RET_GENERIC, "no equipment or location .");
  }

  return TRC_GetAlarm();
}

int MCSMethodClearLotLoc(char *Carrierid) {
  char mcsLocation[_SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1];
  char Amhsequipid[_SIZ_AMHSEQUIPID];
  size_t lotId_len = strlen(Carrierid);
  size_t mcsLoc_len = _SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1;
  DbgPrintf(stderr,"%s:[MCSMethodSendLotLoc]Carrier id:%s\n", TRC_GetTime(), Carrierid);
  if (strlen(Carrierid) == 0) {
    return TRC_SetAlarm("ClearLotLocation", _RET_GENERIC, "Lot Id is empty, fail to move lot location.");
  }

  MCSMethodGetLocFromPromis(Carrierid, Amhsequipid, mcsLocation);

  if (TRC_ChkAlarm()) {
    //update Promis global section
    sprintf(mcsLocation, "%s,Transit", Amhsequipid);
    mcsLoc_len = strlen(mcsLocation);
    //update PROMIS Global Section
    updatePROMISForEM(Carrierid, mcsLocation, &lotId_len, &mcsLoc_len);
    //Update ROMARIC MCS
    if (checkHSMSState()) {
      return TRC_SetAlarm("ClearLotLocation", _RET_GENERIC, "Can't connect to ROMARIC MCS.");
    }
    HSMS_Send(message.buffer);
  }
  return (TRC_GetAlarm());

}

int MCSMethodSendLotLoc(char *Carrierid, char *Amhsequipid, char *Category) {

  char mcsLocation[_SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1];
  DbgPrintf(stderr,"%s:[MCSMethodSendLotLoc]Carrier id:%s,Eqpt id:%s,location:%s,\n", TRC_GetTime(), Carrierid, Amhsequipid, Category);

  if (strlen(Carrierid) == 0) {
    return TRC_SetAlarm("SendLotLocation", _RET_GENERIC, "Lot Id is empty, fail to move lot location.");
  }

  if (strlen(Amhsequipid) == 0) {
    return TRC_SetAlarm("SendLotLocation", _RET_GENERIC, "equipment id empty, fail to move lot location.");
  }

  sprintf(mcsLocation, "%s,%s%s", Amhsequipid, Category, "");
  //update PROMIS Global Section
  size_t lotId_len = strlen(Carrierid);
  size_t mcsLoc_len = mcsLoc_len = strlen(mcsLocation);  //_SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1;
  updatePROMISForEM(Carrierid, mcsLocation, &lotId_len, &mcsLoc_len);
  //Update ROMARIC MCS
  if (checkHSMSState()) {
    return TRC_SetAlarm("SendLotLocation", _RET_GENERIC, "Can't connect to ROMARIC MCS.");
  }
  HSMS_Send(message.buffer);
  return (TRC_GetAlarm());
}
/*************************************************************************/
int MCSMethodMoveLotLoc(char *Carrierid, char *Eqptid, char *Location, char *UserID, char *Priority, int *mcsStatus,
                        char *ErrorMsg) {

  int FABNo;
  DbgPrintf(stderr,"%s:[MCSMethodMoveLotLoc]UserID:%s,Carrier id:%s,Eqpt id:%s,Location:%s,\n", TRC_GetTime(), UserID, Carrierid, Eqptid, Location);

  if (strlen(Carrierid) == 0) {
    return TRC_SetAlarm("MoveLotLocation", _RET_GENERIC, "Lot Id is empty, fail to move lot location.");
  }
  if (strlen(Eqptid) == 0) {
    return TRC_SetAlarm("MoveLotLocation", _RET_GENERIC, "equipment id empty, fail to move lot location.");
  }
  if (strlen(Location) == 0) {
    return TRC_SetAlarm("MoveLotLocation", _RET_GENERIC, "Location is empty, fail to move lot location.");
  }

  FABNo = getStockerFabNo(Eqptid);

  DbgPrintf(stderr,"%s:Stocker:%s belong to %d\n", TRC_GetTime(), Eqptid, FABNo);

  if (FABNo == FAB2) {
    /*...send to ROMARIC MCS*/
    if (checkHSMSState()) {
      return TRC_SetAlarm("MoveLotLocation", _RET_GENERIC, "Can't connect to ROMARIC MCS.");
    }
    HSMS_Send(message.buffer);

  } else if (FABNo == FAB3) {
    /*...send to TOSHIBA MCS...*/
    MCSMethodSendFab3Cmd_NVFEI();
  } else if (FABNo == FAB5) {
    /*...send to TOSHIBA MCS...*/
    MCSMethodMoveLotLocForPRI(Carrierid, Eqptid, Location, UserID, Priority, mcsStatus, ErrorMsg);
  } else {
    sprintf(ErrorMsg, "Lot:%s is not belong to F235, fail to move lot", Carrierid);
    return TRC_SetAlarm("MoveLotLocation", _RET_GENERIC, ErrorMsg);
  }

  return TRC_GetAlarm();

}
/**
 * send move(VFEI) command to Transnet from EM
 */
int MCSMethodMoveLotLocForPRI(char *Carrierid, char *Eqptid, char *Location, char *UserID, char *Priority,
                              int *mcsStatus, char *ErrorMsg) {
  char cmd[_SIZ_GENFIELD];
  int ecd;

  TRC_ClrAlarm();

  PRS_VFI_InitCommand("MACH_CMD", C5SYNC_NAME);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_CMDTYP, _SIZ_CMDTYPE, "MOVE");
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_CARRIERID, _SIZ_CARRIERID, Carrierid);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_DESTID, _SIZ_DESTID, Eqptid);
  /*
   ************************************************************************************
   ***  Put Operator Id as part of move command
   ***/
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_USERID, _SIZ_USERID, UserID);
  /***
   ***  Put Operator Id as part of move command
   ************************************************************************************/

  /*
   | if Location is empty ||, it is a store cmd.  put OUTPUTPORT=""
   | if Location is "NONE", it is a store cmd. put OUTPUTPORT=""
   | If Location is passed as a string, pass it on as _TOK_OUTPUTPORT = <Location>
   */
  /* Location or category contains
   1. |OUTPUT| - is a retrieve command
   2. |PORT-A| - is a retrieve command
   3. ||       - is a store (into stocker) command
   4. |NONE|   - is a store (into stocker) command
   */
  if (strlen(Location) > 0) {
    if (strcmp(Location, "OUTPUT") == 0)
      strcpy(Location, "PORT");  // set to default port(else parsed value)
    else if (strcmp(Location, "NONE") == 0)
      memset(Location, 0, sizeof(Location));
  }

  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_OUTPUTPORT, _SIZ_OUTPUTPORT, Location);

  message.msgType = _TLK_TYPVFEI;
  if (TLK_DMQ_ClientPut(hC5SyncServer) != _RET_SUCCESS)
    return (TRC_GetAlarm());

 /* if (TLK_DMQ_ClientGet(hC5SyncServer) != _RET_SUCCESS) {
    *mcsStatus = 4;
    strcpy(ErrorMsg, "C5MCSSync timeout ");
    return (TRC_GetAlarm());
  }


   * Log failure to the logfile handle.

  if (PRS_VFI_ChkStatus() != _RET_SUCCESS) {
    TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n", TRC_GetTime(), _RET_FAILURE,
             "MOVE CMD FAILED AT VFEI LEVEL");
    return (TRC_GetAlarm());
  }

  if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS)
    return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "CMD"));

  if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0)
    return (TRC_SetAlarm("MOVE", _RET_CMDWRONG, cmd, "CMD_ACK"));

  if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS)
    return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "ECD"));

  if (ecd != 0) {
    *mcsStatus = ecd;
    PRS_VFI_GetToken("ETX", _SIZ_GENFIELD, ErrorMsg);
  }*/
  return (TRC_GetAlarm());
}

int MCSMethodSendFab3Cmd_NVFEI() {
  int Status = -1;
  char Msg[_SIZ_GENFIELD] = { 0 };
  /**************************************************************************************************
   *****  MCS interaction ... send the deli message to MCSsrv, no changes required, no need to parse
   *****  it either.
   *****/
  Status = TLK_DMQ_ClientPut(hToshibaServer);
  if (Status != _RET_SUCCESS) {
    sprintf(Msg, "%d %s", Status, TRC_ErrorMsg());
    TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: DMQ send to MCSsrv returned status [%d]\n%s\n", TRC_GetTime(), Status, Msg);
    //PRS_DEL_InitCommand("MCS_FAILURE", Msg);
  }
  /**************************************************************************************************
   *****  We read the reply off the queue, so that it don't get chocked full...
   *****/
  Status = TLK_DMQ_ClientGet(hToshibaServer);
  /*------- Send whatever comes to MBX serv ----------*/
  if (Status != _RET_SUCCESS) {
    sprintf(Msg, "%d %s", Status, TRC_ErrorMsg());
    TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: MCSsrv DMQGet Status = %d\n%s\n", TRC_GetTime(), Status, Msg);
    // PRS_DEL_InitCommand("MCS_FAILURE", Msg);
  }

  return TRC_GetAlarm();
}

/*.. end func .....................................................................*/

