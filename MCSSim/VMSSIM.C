/*!cc.........................................
 */
/*... Removed the redundant vmssim.h file ....*/
/*............................................*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lotlist.h"
/* The following are standard include modules */
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

TLK_dmqRec *dmqRec;

typedef struct {
  char attach[20];
  char ConfigName[256];
  char logfile[256];
  char qryfreetext[50];
  char servername[20];
  char successfreetext[80];
  char target[20];
  int locid;
  int eqptid;
} SrvId;

SrvId config;

#define LOGNAME                 "stderr"
#define TEMP_QUEUE_NAME         "temp"
#define LOGFILE                 "TstLog"

#define TIMEOUT                 30
#define hPRIMARY                1
#define hREPLY                  2
#define hTARGET                 3
#define _SIZ_CARRIERID          13
#define _SIZ_LOCATIONID         25
#define _MAX_300                300
#define _MAX_750                750

void listDb(void);
void vfeiCmdAck(int errCode, char *errMsg);
void MachCmd(void);
void EventReport(void);
void vfeiCmd(char *SrvCmd);
void McsClrLotLoc(void);
void McsSendLotLoc(void);
void McsGetLotAll(void);
void McsGetLotLoc(void);
void McsMoveLotLoc(void);
void McsSuccess(void);
void deliCmd(void);
char *rTrim(char *tStr);
void Usage(char *cmd);
void ReadConfig(void);
int HandleOptions(int num, char **parms);
void ShutDownSrv(void);
void setQueue(void);
char to_lower(char ch);

int main(int argc, char **argv) {
  char SrvCmd[_SIZ_MSGBUFF] = { 0 };
  char ClientMsg[_SIZ_MSGBUFF] = { 0 };
  char ErrorMsg[_SIZ_GENFIELD] = { 0 };
  int Status = -1, firstOpt = 0;
  int i = 0;

  firstOpt = HandleOptions(argc, argv);
  if (1 == firstOpt) {
    Usage(argv[0]);
    exit(1);
  }

  ReadConfig();

  if (TRC_OpenEx(LOGFILE, LOGNAME, 9, _TRC_NEXT_DAY, _LOG_2MB_LIMIT) != _RET_SUCCESS) {
    fprintf(stderr, "%s: Test program error: Could not open log file [%s]. Quit.\n", TRC_GetTime(), "stderr");
    TRC_Shutdown();
    exit(1);
  }

  /**** Initialize Alarm Events ****/
  if (TRC_InitAlarm(LOGFILE, config.servername, ShutDownSrv) != _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s:FATAL ERROR: Could not Init alarm - program exiting.\n", TRC_GetTime());
    TRC_Shutdown();
    exit(1);
  }
  TRC_Send(LOGFILE, _TRC_LVL_WARN, "%s: Logfile and Alarm Init completed. Setting Queues...\n", TRC_GetTime());
  setQueue();

  LoadLotList();

  do {
    TRC_Send(LOGFILE, _TRC_LVL_INFO, "%s: Waiting for next command:\n", TRC_GetTime());

    Status = TLK_DMQ_ServerGet(hPRIMARY, _TLK_TYPDEL, ClientMsg);
    if (Status != _RET_SUCCESS)
      continue; /* Skip to begining of loop */

    SrvCmd[0] = '\0';
    Status = PRS_VFI_GetToken(_PRS_VFI_CMDID, _SIZ_GENFIELD, SrvCmd);
    if (Status == _RET_SUCCESS)
      vfeiCmd(SrvCmd);
    else
      deliCmd();
  } while (1);
  ShutDownSrv();
  return 0;
}
/************************** End of PRIMain *********************************************/

void setQueue(void) {
  int Status = 0;

  Status = TLK_DMQ_SetQueue(hPRIMARY, config.attach, PSYM_ATTACH_BY_NAME, LOGFILE, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: TEST PROGRAM successfully set queue to [%s.%d]\n", TRC_GetTime(),
             config.attach, hPRIMARY);
  } else {
    TRC_Send(LOGFILE, _TRC_LVL_ERROR, "%s: TEST PROGRAM Error setting queue to [%s.%d]\n", TRC_GetTime(), config.attach,
             hPRIMARY);
    TRC_Shutdown();
    TLK_DMQ_Shutdown();
    exit(1);
  }
  Status = TLK_DMQ_SetQueue(hREPLY, TEMP_QUEUE_NAME, PSYM_ATTACH_TEMPORARY, LOGFILE, _TRC_LVL_WARN);
  if (Status == _RET_SUCCESS) {
    TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: TEST PROGRAM successfully set queue to [%s.%d]\n", TRC_GetTime(),
             TEMP_QUEUE_NAME, hREPLY);
  } else {
    TRC_Send(LOGFILE, _TRC_LVL_ERROR, "%s: TEST PROGRAM Error setting queue to [%s.%d]\n", TRC_GetTime(),
             TEMP_QUEUE_NAME, hREPLY);
    TRC_Shutdown();
    TLK_DMQ_Shutdown();
    exit(1);
  }
}

void ShutDownSrv(void) {
  if (LotIdstart)
    LotListMemFree();
  TRC_Shutdown();
  TLK_DMQ_Shutdown();
  fprintf(stderr, "\n%s: Shutting down server...\n", TRC_GetTime());
  fprintf(stderr, "\n%s: Exit.\n\n", TRC_GetTime());
  exit(0);
}

int HandleOptions(int num, char **parms) {
  int n;

  for (n = 1; n < num; ++n) {
    switch (parms[n][1]) {
      case 'f':
      case 'F':
        if (!parms[n][2])
          strcpy(config.ConfigName, parms[++n]);
        else
          strcpy(config.ConfigName, &parms[n][2]);
        break;
      case 'h':
      case 'H':
      case '?':
        Usage(parms[0]);
        break;
    }
  }
  return n;
}

void ReadConfig(void) {
  FILE *fin;
  char instr[_SIZ_GENFIELD + 1], pstr[_SIZ_GENFIELD + 1];
  char *p1, *p2, *p3;
  fin = fopen(config.ConfigName, "r");
  if (fin) {
    while (fgets(instr, _SIZ_GENFIELD, fin)) {
      strcpy(pstr, instr);
      printf("-->%s\n", pstr);
      if (instr[0] == '#')
        continue;
      if (instr[0] == '[')
        continue;
      p3 = p1 = strtok(instr, " =\t\n\a");
      if (p1 == NULL)
        continue;
      p2 = strtok(NULL, " =\t\n\a");
      if (p2 == NULL)
        continue;

      while (*p3)
        *p3++ = to_lower(*p3);
      if (strcmp(p1, "servername") == 0)
        strcpy(config.servername, p2);
      else if (strcmp(p1, "attach") == 0)
        strcpy(config.attach, p2);
      else if (strcmp(p1, "target") == 0)
        strcpy(config.target, p2);
      else if (strcmp(p1, "logfile") == 0)
        strcpy(config.logfile, p2);
      else if (strcmp(p1, "qryfreetext") == 0)
        strcpy(config.qryfreetext, p2);
      else if (strcmp(p1, "successfreetext") == 0)
        strcpy(config.successfreetext, p2);
      printf("-->%s=%s<--\n", p1, p2);
    }
    fclose(fin);
    config.locid = 0;
    config.eqptid = 0;
    fprintf(stderr, "Current Config:\nServerName = %s\nAttach = %s\nTarget = %s\nConfigName = %s\nLogname = %s\n",
            config.servername, config.attach, config.target, config.ConfigName, config.logfile);
    fprintf(stderr, "Query Text = %s\nSuccess Text = %s\nLoc Id = %d\nEqpit Id = %d\n", config.qryfreetext,
            config.successfreetext, config.locid, config.eqptid);
  } else {
    fprintf(stderr, "%s: Cannot find Simulator config file [%s] - aborting\n", TRC_GetTime(), config.ConfigName);
    exit(-1);
  }
}

void Usage(char *cmd) {
  fprintf(stderr, "\nUsage: %s -fconfigfile\n", cmd);
  fprintf(stderr, "WHERE\n\t-fconfigfile\t  - configfile=Config File with path\nQuit.\n\n");
}

char *rTrim(char *tStr) {
  int iCnt;
  iCnt = strlen(tStr) - 1;
  for (; tStr[iCnt] == 32; --tStr)
    tStr[iCnt] = 0;
  return tStr;
}

char to_lower(char chr) {
  if (chr >= 'A' && chr <= 'Z')
    chr -= ' ';
  return chr;
}

/*... delimited commands ..................................................................*/

void deliCmd(void) {
  char SrvCmd[_SIZ_GENFIELD];
  TRC_ClrAlarm();
  message.msgType = _TLK_TYPDEL;
  PRS_DEL_SetDelim(message.buffer[0]);

  PRS_DEL_GetNext(SrvCmd, _SIZ_GENFIELD);
  PRS_DEL_GetNext(SrvCmd, _SIZ_GENFIELD);
  if (!TRC_ChkAlarm())
    return;
  TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: Processing (NON-VFEI) [%s]\n", TRC_GetTime(), SrvCmd);

  TRC_ClrAlarm();

  if (strcmp(SrvCmd, "MCS_GET_LOT_LOC") == 0)
    McsGetLotLoc();
  else if (strcmp(SrvCmd, "MCS_CLR_LOT_LOC") == 0)
    McsClrLotLoc();
  else if (strcmp(SrvCmd, "MCS_SEND_LOT_LOC") == 0)
    McsSendLotLoc();
  else if (strcmp(SrvCmd, "MCS_GET_LOT_ALL") == 0)
    McsGetLotAll();
  else if (strcmp(SrvCmd, "MCS_MOVE_LOT_LOC") == 0)
    McsMoveLotLoc();
  else
    TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: UnknownCmd[%s]\n", TRC_GetTime(), SrvCmd);
}

void McsSuccess(void) {
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_SUCCESS", config.successfreetext);
  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
}
void McsFail(void) {
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_NOTFND", "Lot is outside the stocker");
  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
}

void McsGetLotLoc(void) {
  char lotId[_SIZ_GENFIELD];
  char tStr[20];

  PRS_DEL_GetNext(lotId, _SIZ_GENFIELD); /*... free text ...*/
  PRS_DEL_GetNext(lotId, _SIZ_GENFIELD); /*... free text ...*/

  TRC_ClrAlarm();

  printf("LotId=%s", lotId);
  if (isLotExist(lotId)) {
    PRS_DEL_InitCommand("MCS_SUCCESS", config.successfreetext);
    sprintf(tStr, "E%07.7d", config.eqptid++);
    PRS_DEL_PutToken("%s", tStr);
    sprintf(tStr, "L%07.7d", config.locid++);
    PRS_DEL_PutToken("%s", tStr);
  } else {
    PRS_DEL_InitCommand("MCS_NOTFND", "Lot is outside the stocker");
  }

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
  if (config.eqptid > 9999999)
    config.eqptid = 1;
  if (config.locid > 9999999)
    config.eqptid = 1;
}

void McsGetLotAll(void) {
  char tStr[_SIZ_MSGBUFF];
  char LotList[_MAX_750][_SIZ_LOTID], tLot[_SIZ_LOTID];
  int totlots = 0, ictr = 0;

  PRS_DEL_GetNext(tStr, _SIZ_GENFIELD); /*... free text ...*/
  PRS_DEL_GetNext(&totlots, _PRS_INT); /*... Num Lots ...*/

  for (ictr = 0; totlots > 0 && (TRC_ChkAlarm()); --totlots, ++ictr)
    PRS_DEL_GetNext(LotList[ictr], _SIZ_LOTID);

  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_SUCCESS", config.successfreetext);
  totlots = ictr;
  PRS_DEL_PutToken("%d", totlots);
  for (ictr = 0; totlots > 0 && (TRC_ChkAlarm()); --totlots, ++ictr) {
    PRS_DEL_PutToken("%s", LotList[ictr]);
    if (isLotExist(LotList[ictr])) {
      sprintf(tStr, "E%07.7d", config.eqptid++);
      PRS_DEL_PutToken("%s", tStr);
      sprintf(tStr, "L%07.7d", config.locid++);
      PRS_DEL_PutToken("%s", tStr);
    } else {
      PRS_DEL_PutToken("%s", "        ");
      PRS_DEL_PutToken("%s", "        ");
    }
    if (config.eqptid > 9999999)
      config.eqptid = 1;
    if (config.locid > 9999999)
      config.eqptid = 1;
  }
  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
}

void McsSendLotLoc(void) {
  McsSuccess();
}

void McsClrLotLoc(void) {
  McsSuccess();
}
void McsMoveLotLoc(void) {
  McsSuccess();
}

/*... vfei commands .......................................................................*/

void vfeiCmd(char *Cmd) {
  TRC_Send(LOGFILE, _TRC_LVL_STATE, "%s: Processing [%s]\n", TRC_GetTime(), Cmd);

  if (strcmp(Cmd, "MACH_CMD") == 0)
    MachCmd();
  else if (strcmp(Cmd, "EVENT_REPORT") == 0)
    EventReport();
  else if (strcmp(Cmd, "ALARM_REPORT") == 0)
    return;
  else if (strcmp(Cmd, "ALARM_SETUP") == 0)
    vfeiCmdAck(0, "");
  else if (strcmp(Cmd, "EVENT_SETUP") == 0)
    vfeiCmdAck(0, "");
  else if (strcmp(Cmd, "STATUS_QUERY") == 0)
    MachCmd();
}

void MachCmd(void) {
  char cmdTyp[_SIZ_CMDTYPE];
  PRS_VFI_GetToken("CMD_TYPE", _SIZ_CMDTYPE, cmdTyp);

  if (strcmp(cmdTyp, "STARTUP") == 0)
    vfeiCmdAck(0, "");
  else if (strcmp(cmdTyp, "HOST_AUTHORIZE") == 0)
    vfeiCmdAck(0, "");
  else if (strcmp(cmdTyp, "MOVE") == 0)
    vfeiCmdAck(0, "");
  else if (strcmp(cmdTyp, "LIST_DB") == 0)
    listDb();
}

void vfeiCmdAck(int errCode, char *errMsg) {
  PRS_VFI_GetToken(_PRS_VFI_TID, _PRS_UINT, &message.seqToken);

  PRS_VFI_InitReply("CMD_ACK", config.servername);
  PRS_VFI_PutToken("ECD", _PRS_UINT, &errCode);
  PRS_VFI_PutToken("ETX", _SIZ_GENFIELD, errMsg);
  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
}

void listDb(void) {
  int pnum = 0, msgLen = 0;
  FILE *rfp;
  char instr[_SIZ_GENFIELD + 1];
  PRS_VFI_GetToken("PAGENUMBER", _PRS_UINT, &pnum);
  PRS_VFI_GetToken(_PRS_VFI_TID, _PRS_UINT, &message.seqToken);

  PRS_VFI_InitReply("STATUS_LIST", config.servername);
  PRS_VFI_PutToken("PAGENUMBER", _PRS_UINT, &pnum);
  pnum = 0;
  PRS_VFI_PutToken("CMD_TYPE", _SIZ_GENFIELD, "DB_LIST");
  if ((rfp = fopen(config.ConfigName, "r")) == NULL) {
    pnum = 1001;
    strcpy(instr, "Error opening status database.");
  } else {
    while (!feof(rfp)) {
      fgets(instr, _SIZ_GENFIELD, rfp);
      if (instr[0] == '#')
        continue;
      if (strncmp(instr, "[DB_LIST]", 9) == 0) {
        while (!feof(rfp)) {
          fgets(instr, _SIZ_GENFIELD, rfp);
          printf("+->%s\n", instr);
          if (instr[0] == '#')
            continue;
          if (strncmp(instr, "[END_DB_LIST]", 13) == 0)
            break;
          message.buffer[message.msgPtr++] = _PRS_VFI_DELIMIT;
          strcpy(&message.buffer[message.msgPtr], instr);
          msgLen = strlen(instr) - 1;
          message.msgPtr += msgLen;
          message.buffer[message.msgPtr] = 0;
          message.msgLen = message.msgPtr + 1;
        }
      }
      if (strncmp(instr, "[END_DB_LIST]", 13) == 0)
        break;
    }
    fclose(rfp);
    pnum = 0;
    memset(instr, 0, sizeof(instr));
  }
  PRS_VFI_PutToken("ECD", _PRS_UINT, &pnum);
  PRS_VFI_PutToken("ETX", _SIZ_GENFIELD, instr);

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hPRIMARY);
}

void EventReport(void) {
  return;
}
