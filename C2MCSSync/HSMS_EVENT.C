#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "unistd.h"

#include "getmcs.h"
#include "trcutil.h"
#include "parsvfei.h"
#include "hsms_event.h"

extern void updatePROMISForMCSMove(char *lotId, char *mcsLocation, size_t * lotId_len, size_t * mcsLoc_len);

void HSMS_Event(const char *s, unsigned int replyflag, unsigned int sb) {
  printf("************Receive ROMARIC HSMS_Event*****\n");
  printf("%s\n************************\n", s);
  char Amhsequipid[EQP_ID_LEN + 1];
  char LotLocId[LOC_ID_LEN + 1];
  char TmpLotId[LOT_LEN + 1];
  char mcsLocation[EQP_ID_LEN + LOC_ID_LEN + 1];
  char Category[_SIZ_CATEGORY] = { 0 };
  char Number[_SIZ_NUMBER] = { 0 };

  char SrvCmd[_SIZ_GENFIELD] = { 0 };
  char Event_Id[_SIZ_GENFIELD] = { 0 };

  int Status = 0;
  message.msgPtr = 0;
  message.buffer = malloc(strlen(s)+1);
  strncpy(message.buffer, s, strlen(s));
  message.msgLen = strlen(s);

  SrvCmd[0] = '\0';
  Status = HSMS_VFI_GetToken(_PRS_VFI_CMDID, _SIZ_GENFIELD, SrvCmd);
  //if VFEI format from ROMARIC MCS
  if (Status != _RET_SUCCESS) {
    printf("Not VFEI Format\n");
    return;
  }
  printf("Command:|%s|\n", SrvCmd);

  if (strcmp(SrvCmd, _EVENT_REPORT) == 0) {
    Event_Id[0] = '\0';
    Status = HSMS_VFI_GetToken(_PRS_VFI_EVENTID, _SIZ_GENFIELD, Event_Id);
    if (Status != _RET_SUCCESS) {
      printf("Error in Parsing Event Report in VFEI\n");
      free(message.buffer);
      return;
    }
    printf("EVENT ID:|%s|\n", Event_Id);

    if (strcmp(Event_Id, "CARRIER_REMOVED") == 0) {
      Status = ParseCarrierRemoved(TmpLotId, Amhsequipid);
      sprintf(mcsLocation, "%s,Transit", Amhsequipid);
    } else {
      if (strcmp(Event_Id, "LOC_CHANGED") == 0 || strcmp(Event_Id, "CARRIER_ENTERED") == 0) {
        Status = ParseCarrierEntered(TmpLotId, Amhsequipid, Category, Number);
        sprintf(mcsLocation, "%s,%s%s", Amhsequipid, Category, Number);
      }
    }

    size_t lotId_len = strlen(TmpLotId);
    size_t mcsLoc_len = strlen(mcsLocation);//EQP_ID_LEN + LOC_ID_LEN + 1;

    printf("Lot ID:%s,MCS Location:%s\n", TmpLotId, mcsLocation);
    if (TmpLotId[0] != 0) {
      updatePROMISForMCSMove(TmpLotId, mcsLocation, &lotId_len, &mcsLoc_len);
    }
  }
  free(message.buffer);

}

/**
 * Start ROMARIC HSMS Thread.
 */
void startHSMS(int * port, int* RetSts, char *ErrMsgC) {
  int x = 50;
  *RetSts = 0;

  HSMS_Start(*port);

  while (GetHSMSState() != HSMS_SELECTED && x >= 0) {
    printf("HSMS is not connected,retry again...%d\n", x--);
    sleep(2);
  }
  if (GetHSMSState() != HSMS_SELECTED) {
    strcpy(ErrMsgC, TRC_GetTime());
    strcat(ErrMsgC, "ROMARIC HSMS is not connected.");
    *RetSts = 1;
  }
}
/**
 * Stop ROMARIC HSMS Thread
 */
void stopHSMS() {
  HSMS_Stop();
}

/**
 * Check if ROMARIC State is online.
 */
void checkHSMSState(int* RetSts, char *ErrMsgC) {
  int x = 50;
  *RetSts = 0;

  while (GetHSMSState() != HSMS_SELECTED) {
    printf("HSMS is not connected,retry again...%d\n", x--);
    sleep(2);
  }
  if (GetHSMSState() != HSMS_SELECTED) {
    strcpy(ErrMsgC, TRC_GetTime());
    strcat(ErrMsgC, "ROMARIC HSMS is not connected.");
    *RetSts = 1;
  }
}
/**
 * Listen ROMARIC MCS Event
 */
int listenHSMS() {
  printf("Loop for Listening to the ROMARIC HSMS Event....\n");
  while (1) {
    // printf("Loop for Listening to the ROMARIC HSMS Event....\n");
    printf("%s:HSMS State:s%\n",TRC_GetTime(),GetHSMSState());
    sleep(2);
  }
  return 0;
}
int ParseCarrierEntered(char *Carrierid, char *Amhsequipid, char *Category, char *Number) {
  int Status = 0;
  char dummynumber[_SIZ_DUMMYNUMBER] = { 0 }, s1[3] = { 0 }, s2[3] = { 0 }, s3[3] = { 0 }, s4[3] = { 0 };

  strcpy(Carrierid, "");
  strcpy(Amhsequipid, "");
  strcpy(Category, "");
  strcpy(Number, "");

  Status = HSMS_VFI_GetToken(_TOK_CARRIERID, _SIZ_LOTID, Carrierid);
//  printf("Carrier id:%s\n", Carrierid);
  if (Status != _RET_SUCCESS) {
    printf("Error in Parsing Lot Id in VFEI\n");
    return 1;
  }
  // printf("Carrier id:%s\n", Carrierid);

  Status = HSMS_VFI_GetToken(_TOK_AMHSEQUIPID, _SIZ_AMHSEQUIPID, Amhsequipid);
  if (Status != _RET_SUCCESS) {
    printf("Error in Parsing Amhsequip id in VFEI\n");
    return 1;
  }
  // printf("Amhs equipid:%s\n", Amhsequipid);

  Status = HSMS_VFI_GetToken(_TOK_CATEGORY, _SIZ_CATEGORY, Category);
  if (Status != _RET_SUCCESS) {
    printf("Error in Parsing Category in VFEI\n");
    return 1;
  }
  //printf("Category:%s\n", Category);

  Status = HSMS_VFI_GetToken(_TOK_NUMBER, _SIZ_DUMMYNUMBER, dummynumber);
  if (Status != _RET_SUCCESS) {
    printf("Error in Parsing dummy number in VFEI\n");
    return 1;
  }
  printf("dummy number:%s\n", dummynumber);

  if (strcmp(Category, "SHELF") == 0) {
    if (strlen(dummynumber) == 13) {
      sscanf(dummynumber, "%3s%2s%1s%3s%1s%3s", s4, s1, s4, s2, s4, s3);
      sprintf(Number, "%s%s%s", s1, s2, s3);
      strcpy(Category, "");
    } else
      strcpy(Number, dummynumber);
  } else if (strcmp(Category, "IOPORT") == 0) {
    strcpy(Category, "");
    strcpy(Number, dummynumber);
  } else
    strcpy(Number, dummynumber);

  return Status;
}

int ParseCarrierRemoved(char *Carrierid, char *Amhsequipid) {
  int Status = 0;
  strcpy(Carrierid, "");
  strcpy(Amhsequipid, "");

  Status = HSMS_VFI_GetToken(_TOK_CARRIERID, _SIZ_LOTID, Carrierid);
  // printf("Paring status in ParseCarrierRemoved:%d\n", Status);
  // printf("Paring Lot ID in ParseCarrierRemoved:%s\n", Carrierid);
  if (Status != _RET_SUCCESS) {
    printf("Error in Parsing Lot Id in VFEI\n");
    return 1;
  }
  Status = HSMS_VFI_GetToken(_TOK_AMHSEQUIPID, _SIZ_AMHSEQUIPID, Amhsequipid);
  // printf("Paring Amhsequipid in ParseCarrierRemoved:%s\n", Amhsequipid);
  return Status;
}
