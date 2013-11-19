/*************************************************************************

 MCSrEPLY:                                      Date:  July 2013
 ~~~~~~~~~~~~~                                     Author: Zhou An-Ping
 **************************************************************************/

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

int MCSReply(char * rHostname) {
  char ErrorMsg[_SIZ_GENFIELD];
  strcpy(message.host, rHostname); /*... put back host name ...*/
  if (TRC_ChkAlarm())
    MCSReplyCmdack_NVFEI();
  else {
    strcpy(ErrorMsg, TRC_ErrorMsg());
    MCSReplyFailure(ErrorMsg);
  }
  return TRC_GetAlarm();
}

int MCSReplyCmdack_VFEI(char *MachId) {
  int ErrorCode;
  char ErrorText[_SIZ_GENFIELD];

  /*........................................................................*/
  /* Generate the message...                                                */
  ErrorCode = TRC_GetAlarm();
  //strcpy(ErrorText, "");
  //if (ErrorCode != _RET_SUCCESS) ...will return null if no error
  strcpy(ErrorText, TRC_ErrorMsg());

  TRC_ClrAlarm();
  PRS_VFI_InitReply(_CMD_ACK, MachId);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_ECD, _PRS_UINT, &ErrorCode);
  if (TRC_ChkAlarm())
    PRS_VFI_PutToken(_TOK_ETX, _SIZ_GENFIELD, ErrorText);
  /*........................................................................*/
  /* Send the reply...                                                      */
  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hMCSINTPrimary);
  return TRC_GetAlarm();
}
/*-----------------------------------------------------------------------------------------*/

int MCSReplyCmdack_NVFEI() {
  int ErrorCode;
  char ErrorText[_SIZ_GENFIELD];

  /*........................................................................*/
  /* Generate the message...                                                */

  ErrorCode = TRC_GetAlarm();
  strcpy(ErrorText, TRC_ErrorMsg());
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_SUCCESS", "F235MCSINT successfully completed command");

  /*........................................................................*/
  /* Send the reply...                                                      */

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hMCSINTPrimary);

  return TRC_GetAlarm();
}
/*-----------------------------------------------------------------------------------------*/

int MCSReplyGetLotLoc(char *Amhsequipid, char *Location) {
  int ErrorCode;
  char ErrorText[_SIZ_GENFIELD];

  /*........................................................................*/
  /* Generate the message...                                                */

  ErrorCode = TRC_GetAlarm();
  strcpy(ErrorText, TRC_ErrorMsg());
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_SUCCESS", "C235MCSINT successfully completed command");
  PRS_DEL_PutToken("%s", Amhsequipid);
  PRS_DEL_PutToken("%s", Location);
  /*........................................................................*/
  /* Send the reply...                                                      */

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hMCSINTPrimary);

  return TRC_GetAlarm();
}
/*-----------------------------------------------------------------------------------------*/

int MCSReplyLotNotFoundInMCS(char *ErrorMsg) {
  int ErrorCode;
  char ErrorText[_SIZ_GENFIELD];

  /*........................................................................*/
  /* Generate the message...                                                */

  ErrorCode = TRC_GetAlarm();
  strcpy(ErrorText, TRC_ErrorMsg());
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_LOT_NOTFND", ErrorMsg);

  /*........................................................................*/
  /* Send the reply...                                                      */

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hMCSINTPrimary);

  return TRC_GetAlarm();
}
/*-----------------------------------------------------------------------------------------*/

int MCSReplyFailure(char *ErrorMsg) {
  int ErrorCode;
  char ErrorText[_SIZ_GENFIELD];

  /*........................................................................*/
  /* Generate the message...                                                */

  ErrorCode = TRC_GetAlarm();
  strcpy(ErrorText, TRC_ErrorMsg());
  TRC_ClrAlarm();
  PRS_DEL_InitCommand("MCS_FAILURE", ErrorMsg);

  /*........................................................................*/
  /* Send the reply...                                                      */

  if (TRC_ChkAlarm())
    TLK_DMQ_ServerPut(hMCSINTPrimary);

  return TRC_GetAlarm();
}
/*-----------------------------------------------------------------------------------------*/
