
/*****************************************************************************

  PRIParseLockGraph :                                   Date:  May 1998
  ~~~~~~~~~~~~~~~~~~                                 Author:  Ben
  
      This function will parse the command line for get batch detail command.
    
   Arguments:
   ~~~~~~~~~~

      - EqptId   (type is char * - Read/Write):
      The referenced variable contains eqptid.
      - EqptType ( type *char  - Read only)
      The referenced variable contains eqpt Type.
      - BatchId  (type is char * - Read/Write):
      The referenced variable contains Batch id.
  
   Returns:
   ~~~~~~~~
      Type is int - coded value representing the completion status of the
      function.  The code will be one of the standard PRIsrv return values.

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
#include "prisrv.h"
 

int PRIParseCarrierEntered( char *Carrierid, char *Amhsequipid, char *Category, char *Number)
{
/*****************************************************************************
****  Changed program to return _RET_FAB3LOCATION or _RET_SMPLOCATION or any
****  error that may have been encountered.
****
   ****/
   char dummynumber[_SIZ_DUMMYNUMBER]={0},s1[3]={0},s2[3]={0},s3[3]={0},s4[3]={0};
   
   TRC_ClrAlarm();
   
   strcpy(Carrierid,"");
   strcpy(Amhsequipid,"");
   strcpy(Category,"");
   strcpy(Number,"");
   
   PRS_VFI_GetToken(_TOK_CARRIERID,_SIZ_LOTID,Carrierid);
   if (TRC_ChkAlarm())
      PRS_VFI_GetToken(_TOK_AMHSEQUIPID,_SIZ_AMHSEQUIPID,Amhsequipid);
   if (TRC_ChkAlarm())
      PRS_VFI_GetToken(_TOK_CATEGORY,_SIZ_CATEGORY,Category);
   if (TRC_ChkAlarm())
      PRS_VFI_GetToken(_TOK_NUMBER,_SIZ_DUMMYNUMBER,dummynumber);
  // fprintf(stderr, "CarrierID %s, Amhsequipid %s, Category %s,dummynumber %s\n", Carrierid,Amhsequipid,Category,dummynumber);
   if (strcmp(Category,"SHELF")==0)
   {
      if (strlen(dummynumber) == 13)
      {
         sscanf(dummynumber,"%3s%2s%1s%3s%1s%3s",s4,s1,s4,s2,s4,s3);
         sprintf(Number,"%s%s%s",s1,s2,s3);
         strcpy(Category,"");
      }
      else
         strcpy(Number,dummynumber);
   }
   else if (strcmp(Category,"IOPORT")==0)
   {
      strcpy(Category,"");
      strcpy(Number,dummynumber);
   }
   else
      strcpy(Number,dummynumber);
   /**---------------------------------------------------------------------**
    **   Check for PROMISLOC = smp or fab3
    **
    **/
   if(TRC_ChkAlarm()) //alarm==_ret_success
   {
      // ChkPromisLoc returns _RET_SMPLOCATION or _RET_FAB3LOCATION or Error
     // return (chkPromisLoc());
   }
   return(TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

int PRIParseCarrierRemoved( char *Carrierid, char *Amhsequipid)
{
   /*****************************************************************************
    **   Changed program to return _RET_FAB3LOCATION or _RET_SMPLOCATION or any
    **   error that may have been encountered.
    **
    **/
   TRC_ClrAlarm();
   
   strcpy(Carrierid,"");
   strcpy(Amhsequipid,"");
   
   PRS_VFI_GetToken(_TOK_CARRIERID,_SIZ_LOTID,Carrierid);
   if (TRC_ChkAlarm()) PRS_VFI_GetToken(_TOK_AMHSEQUIPID,_SIZ_AMHSEQUIPID,Amhsequipid);
   /***********************************************************************
    **   Check for PROMISLOC = smp or fab3
    **
    **/
   if(TRC_ChkAlarm()) //alarm==_ret_success
   {
      // ChkPromisLoc returns _RET_SMPLOCATION or _RET_FAB3LOCATION or error
     // return (chkPromisLoc());
   }
   return(TRC_GetAlarm()); //return any error that was encontered
}
/*-----------------------------------------------------------------------------------------*/


int PRIParseMid( char *Mid)
{
   
   TRC_ClrAlarm();
   
   strcpy(Mid,"");
   PRS_VFI_GetToken(_TOK_MID,_SIZ_MID,Mid);
   return(TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/

int PRIParseCarrierid( char *Carrierid)
{
   
   TRC_ClrAlarm();
   
   strcpy(Carrierid,"");
   PRS_DEL_GetNext(Carrierid,_SIZ_LOTID);
   PRS_DEL_GetNext(Carrierid,_SIZ_LOTID);
   return(TRC_GetAlarm());
}
/*-----------------------------------------------------------------------------------------*/


