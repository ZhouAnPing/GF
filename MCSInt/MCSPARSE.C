/*****************************************************************************

 MCSParse :                                   Date:  July 2013
 ~~~~~~~~~~~~~                                     Author: Zhou An-Ping

 This function will parse the command message for get batch detail command.

 Modifications:
 ~~~~~~~~~~~~~~
 - Author                     Date            Description...

 *****************************************************************************/

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

int MCSParseSetLotLocation_NVFEI(char *Carrierid, char *Amhsequipid, char *Category) {

  TRC_ClrAlarm();

  strcpy(Carrierid, "");
  strcpy(Amhsequipid, "");
  strcpy(Category, "");
  PRS_DEL_GetNext(Carrierid, _SIZ_LOTID);
  PRS_DEL_GetNext(Carrierid, _SIZ_LOTID);
  if (TRC_ChkAlarm())
    PRS_DEL_GetNext(Amhsequipid, _SIZ_AMHSEQUIPID);
  if (TRC_ChkAlarm())
    PRS_DEL_GetNext(Category, _SIZ_CATEGORY);
  return (TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

int MCSParseCarrierid(char *Carrierid) {

  TRC_ClrAlarm();

  strcpy(Carrierid, "");
  PRS_DEL_GetNext(Carrierid, _SIZ_LOTID);
  PRS_DEL_GetNext(Carrierid, _SIZ_LOTID);
  return (TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

int MCSParseMoveLotLoc(char *UserID, char *Carrierid, char *Eqptid, char *Location, char *Priority) {

  TRC_ClrAlarm();

  strcpy(UserID, "");
  strcpy(Carrierid, "");
  strcpy(Eqptid, "");
  strcpy(Location, "");
  strcpy(Priority, "");
  PRS_DEL_GetNext(UserID, _SIZ_USERID);
  PRS_DEL_GetNext(UserID, _SIZ_USERID);
  PRS_DEL_GetNext(Carrierid, _SIZ_LOTID);
  PRS_DEL_GetNext(Eqptid, _SIZ_EQPTID);
  PRS_DEL_GetNext(Location, _SIZ_CATEGORY);
  PRS_DEL_GetNext(Priority, _SIZ_MCSPRIORITY);
  return (TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

int MCSParseMid( char *Mid)
{

   TRC_ClrAlarm();

   strcpy(Mid,"");
   PRS_VFI_GetToken(_TOK_MID,_SIZ_MID,Mid);
   return(TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

