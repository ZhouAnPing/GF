/*******************************************************************************

 GET_MCS_INFO                                  Date:  FEB   1996
 ~~~~~~~~~~~~~                                 Author:Jayaraman Subramanian

 For a given list of lots, this functions tries to get equipment id
 and location from MCS system.

 Arguments:
 ~~~~~~~~~~
 number of lots:    Specifies the number of lots passed on to
 this routine  ( input)


 lot id        :    List of lots for which eqp.id and location
 will be requested.

 Location      :    Returns the Eqpid and Location id in this
 variable back to calling function


 RETURN STATUS: This return the completion status of this
 function: 0 Indicates success
 1 indicates failure

 ERROR MESSAGE: This variable contains the error message if the
 return status is 1

 Returns:
 ~~~~~~~~
 NONE

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date
 Description...

 *******************************************************************************/
/*****************************************************************************/
/* The following are standard C include statements.                          */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/******************************************************************************/
/* The following are include statements for local files.                      */

#include "dmqutil.h"
#include "talkstd.h"
#include "trcutil.h"
#include "getmcs.h"
#include "hsms_event.h"

/*int HSMS_SendAndWait(const char * s, char * rs, int rsbuf);
 void checkHSMSState(int* RetSts, char *ErrMsgC);*/
/******************************************************************************/
/* Declaration of global variables with file scope.                           */
DMQ_parms dmq;
void get_mcs_info(int *num_lots, char LotId[][LOT_LEN], char McsLocation[][EQP_ID_LEN + LOC_ID_LEN + 1], int *RetSts,
                  char *ErrMsgC) {

  DMQ_pckt packet;
  DMQ_pckt retPacket;
  int TmpVar, RecvLots;
  char Ptr[80];
  char LotEqpId[EQP_ID_LEN + 1];
  char LotLocId[LOC_ID_LEN + 1];
  char TmpLotId[LOT_LEN + 1];
  char *receivedMsg;  //[_DMQ_BUFFSZ];
  char *sendMsg;  //
  int LotIdPos;
  int EqpLen;

  *RetSts = 0;

  strcpy(ErrMsgC, " GET MCS LOCATION :- SUCCESS");

  /* Expect Reply from server */

  packet.type = _DMQ_TYP_REPLY;

  /* Write Header.   Request is MCS_GET_LOT_ALL */

  TLK_PutMsgHeader(&packet, "MCS_GET_LOT_ALL", "VMS MCS Client query", '|');

  sprintf(Ptr, "%d", *num_lots);

  /* Number of lot ids */

  TLK_PutNextField(&packet, "%s", Ptr);

  /* Array of Lot ids put to packet to be sent to server */

  for (TmpVar = 0; TmpVar < *num_lots; TmpVar++) {
    strncpy(TmpLotId, LotId[TmpVar], LOT_LEN);
    TmpLotId[LOT_LEN] = (char) NULL;

    for (LotIdPos = 0; LotIdPos < LOT_LEN; LotIdPos++) {
      if (TmpLotId[LotIdPos] == ' ') {
        TmpLotId[LotIdPos] = (char) NULL;
        break;
      }
    }
    TLK_PutNextField(&packet, "%s", TmpLotId);
  }

  //test program begin

  /*const char string[] =
   "|MCS_SUCCESS|ROMARIC MCS Simulator reply successfully|10|3AAZ42064.1|E0000001|L0000001|3AAZ42065.1|E0000002|L0000002|3ASZ43111.1|E0000003|L0000003|3ASZ43112.1|E0000004|L0000004|3ASZ43113.1|E0000005|L0000005|3ASZ43114.1|E0000006|L0000006|3ASZ43115.1|E0000007|L0000007|3ASZ43116.1|E0000008|L0000008|3ASZ43117.1|E0000009|L0000009|3ASZ43118.1|E0000010|L0000010|";

   strncpy(retPacket.buffer, string, strlen(string));
   retPacket.buffLen = strlen(string);
   retPacket.buffPtr = 1;

   int retLots = 0;
   char message[12];
   printf("Begin paring reply string %s\n", retPacket.buffer);

   if (TLK_GetFirstField(&retPacket, &retLots, _TLK_INT) != _TLK_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, "Error in Decoding Reply ");
   *RetSts = 1;
   return;
   }
   printf("Return Lot count:%d\n", retLots);
   for (TmpVar = 0; TmpVar < retLots; TmpVar++) {

   if (TLK_GetNextField(&retPacket, Ptr, sizeof(Ptr)) != _TLK_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, "Error in getting Field from reply- Lot id ");
   *RetSts = 1;
   return;
   }
   printf("Return Lot Id:%s-sizeof(Ptr)=%d", Ptr,sizeof(Ptr));
   if (TLK_GetNextField(&retPacket, LotEqpId, EQP_ID_LEN) != _TLK_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, "Error in getting Field from reply- equipment id ");
   *RetSts = 1;
   return;
   }
   printf(",Lot EqpId:%s", LotEqpId);
   if (TLK_GetNextField(&retPacket, LotLocId, LOC_ID_LEN) != _TLK_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, "Error in getting Field from reply- location id ");
   *RetSts = 1;
   return;
   }
   printf(",Lot LocId:%s\n", LotLocId);

   }

   return;*/
  //end test program
  checkHSMSState(RetSts, ErrMsgC);
  if (*RetSts == 1) {
   // strcpy(ErrMsgC, TRC_GetTime());
    //strcat(ErrMsgC, "Error J");
    *RetSts = 1;
    return;
  }

  receivedMsg = malloc(_DMQ_BUFFSZ);
  sendMsg = malloc(packet.buffLen + 1);
  strncpy(sendMsg, packet.buffer, packet.buffLen);

  if (!HSMS_SendAndWait(sendMsg, receivedMsg, _DMQ_BUFFSZ)) {
    strcpy(ErrMsgC, TRC_GetTime());
    strcat(ErrMsgC, "error happen in invoking HSMS_SendAndWait method");
    free(receivedMsg);
    free(sendMsg);
    *RetSts = 1;
    return;
  }
  printf("***********received Message:%s\n",receivedMsg);
  strncpy(retPacket.buffer, receivedMsg, strlen(receivedMsg));
  retPacket.buffLen = strlen(receivedMsg);
  retPacket.buffPtr = 1;

  free(receivedMsg);
  free(sendMsg);

  /*  if (TLK_CltSubmit(&dmq, &packet) != _DMQ_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, DMQ_ErrorMsg);
   *RetSts = 1;
   return;
   } else if (TLK_CltReceive(&dmq, &packet) != _DMQ_SUCCESS) {
   strcpy(ErrMsgC, TRC_GetTime());
   strcat(ErrMsgC, DMQ_ErrorMsg);
   *RetSts = 1;
   return;
   }*/

  /* From Reply Get first field .i.e. The number of lots received */

  if (TLK_GetFirstField(&retPacket, &RecvLots, _TLK_INT) != _TLK_SUCCESS) {
    strcpy(ErrMsgC, TRC_GetTime());
    strcat(ErrMsgC, "Error in Decoding Reply ");
    *RetSts = 1;
    return;
  }

  if (RecvLots != *num_lots) {
    strcpy(ErrMsgC, TRC_GetTime());
    strcat(ErrMsgC, "Number of lots less than Expected lots ");
    *RetSts = 1;
    return;
  }

  for (TmpVar = 0; TmpVar < *num_lots; TmpVar++) {

    //for HSMS format, the lot id will be not return.
    /* if (TLK_GetNextField(&retPacket, Ptr, sizeof(Ptr)) != _TLK_SUCCESS) {
     strcpy(ErrMsgC, TRC_GetTime());
     strcat(ErrMsgC, "Error in getting Field from reply- Lot id ");
     *RetSts = 1;
     return;
     }
     printf("Return Lot Id:%s", Ptr);*/
    if (TLK_GetNextField(&retPacket, LotEqpId, EQP_ID_LEN) != _TLK_SUCCESS) {
      strcpy(ErrMsgC, TRC_GetTime());
      strcat(ErrMsgC, "Error in getting Field from reply- equipment id ");
      *RetSts = 1;
      return;
    }
    //printf(",Lot EqpId:%s", LotEqpId);
    if (TLK_GetNextField(&retPacket, LotLocId, LOC_ID_LEN) != _TLK_SUCCESS) {
      strcpy(ErrMsgC, TRC_GetTime());
      strcat(ErrMsgC, "Error in getting Field from reply- location id ");
      *RetSts = 1;
      return;
    }
    // printf(",Lot LocId:%s\n", LotLocId);
    EqpLen = strlen(LotEqpId);

    strncpy(McsLocation[TmpVar], _ALL_SPACES, EQP_ID_LEN + LOC_ID_LEN + 1);
    strncpy(McsLocation[TmpVar], LotEqpId, EqpLen);

    if (!all_space(LotEqpId))
      McsLocation[TmpVar][EqpLen] = ',';
    else
      McsLocation[TmpVar][EqpLen] = ' ';

    strncpy(&McsLocation[TmpVar][EqpLen + 1], LotLocId, strlen(LotLocId));

  }
  return;
}

/****************************END OF FUNCTION get_mcs_info ************/
