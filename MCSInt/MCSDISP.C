/*************************************************************************

 MCSDispatch:                                      Date:  July 2013
 ~~~~~~~~~~~~~                                     Author: Zhou An-Ping

 This function is the router for processing the MCS Int server's input
 commands.It determines which function should be called based on the
 contents of the inbound message's header.

 Arguments:
 ~~~~~~~~~~
 None.

 Returns:
 ~~~~~~~~
 Nothing.

 Modifications:
 ~~~~~~~~~~~~~~
 Author           Date            Description...
 ---------------------------------------------------------------------------
 Zhou An-Ping     07-25-2013      Inital Version

 **************************************************************************/

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

int Dispatch_Deli(char *Cmd) {
  char Carrierid[_SIZ_CARRIERID];
  char Amhsequipid[_SIZ_AMHSEQUIPID];
  char Category[_SIZ_CATEGORY];
  char Location[_SIZ_CATEGORY];
  char Mid[_SIZ_MID];
  char mcsLocation[_SIZ_AMHSEQUIPID + _SIZ_CATEGORY - 1];
  char Eqptid[_SIZ_EQPTID];
  char UserID[_SIZ_USERID]; /* New */
  char Priority[_SIZ_MCSPRIORITY];
  char *tmpEQPId;
  char *RomaricMsg;
  int mcsstatus;
  char ErrorMsg[_SIZ_GENFIELD];
  char rHostname[_SIZ_MID] = { 0 };
  extern int transnet_down;

  TRC_ClrAlarm();
  if (transnet_down) {
    strcpy(ErrorMsg, "Transnet is down ");
    MCSReplyFailure(ErrorMsg);
    return (0);
  }
  /*... Save host information ...*/
  strcpy(rHostname, message.host);

  if (strcmp(Cmd, "MCS_GET_LOT_LOC") == 0) {
    /* Perform Get Lot from MCS */
    MCSParseCarrierid(Carrierid);

    mcsstatus = 1;
    if (TRC_ChkAlarm())
      MCSMethodGetLotLoc(Carrierid, Amhsequipid, Location, &mcsstatus, ErrorMsg);

    strcpy(message.host, rHostname); /*... put back host name ...*/
    if (mcsstatus)
      MCSReplyGetLotLoc(Amhsequipid, Location);
    if (!mcsstatus)
      MCSReplyLotNotFoundInMCS(ErrorMsg);
    return (TRC_ChkAlarm());

  } else if (strcmp(Cmd, "MCS_MOVE_LOT_LOC") == 0) {
    /* send Move command to MCS */
    MCSParseMoveLotLoc(UserID, Carrierid, Eqptid, Location, Priority); /* add UserID */

    MCSMethodMoveLotLoc(Carrierid, Eqptid, Location, UserID, Priority, &mcsstatus, ErrorMsg);

    MCSReply(rHostname);
  } else if (strcmp(Cmd, "MCS_SEND_LOT_LOC") == 0) {
    /* Send message to MCS */
    MCSParseSetLotLocation_NVFEI(Carrierid, Amhsequipid, Category);

    MCSMethodSendLotLoc(Carrierid, Amhsequipid, Category);

    MCSReply(rHostname);
  } else if (strcmp(Cmd, "MCS_CLR_LOT_LOC") == 0) {
    // clear lot location
    MCSParseCarrierid(Carrierid);

    MCSMethodClearLotLoc(Carrierid);

    MCSReply(rHostname);
  } else {
    //MCSParseMid(Mid);
    TRC_SetAlarm("MCSDispatch()", _RET_CMDINVALID, Cmd);
    strcpy(message.host, rHostname); /*... put back host name ...*/
    MCSReplyCmdack_NVFEI();
    return (TRC_SetAlarm("F235MCSINTSrv received ", _RET_FAILURE, "UNKNOWN CMD"));
  }

  return (TRC_ChkAlarm());
}

