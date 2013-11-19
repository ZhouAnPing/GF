/********************************************************************************************/
/*                                                                       ********************/
/*   primethod.c:                                    Date:  October 2000 ********************/
/*   ~~~~~~~~~~~~~                                                       ********************/
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
#include "prisrv.h"
#define  _MAX_300    300 

int PRIMethodCarrierEntered( char *Carrierid,char *Amhsequipid,char *Category,
                            char *Number,int *ptpstatus,char *ErrorMsg)
{
   char PromUsrId[_SIZ_USERID];
   char PromPasswd[_SIZ_PASSWORD];
   char CarrierEntered[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=0;
   
   GetPromUsrPwd(PromUsrId, PromPasswd);
   sprintf ( CarrierEntered,"%s,%s%s",Amhsequipid,Category,Number);
//   TRC_Send( LOG_ALIAS, _TRC_LVL_INFO,
//      "%s:Exec carrier entered\n [%s][%s][%s][%s][%s]\n",
//      TRC_GetTime(), Carrierid, Amhsequipid, Category, Number,CarrierEntered);
   if (TRC_ChkAlarm()) PRS_PTP_InitCommand (_PRS_PTP_UPDATEPARAM);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID %s", PromUsrId);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PWD %s", PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("LOTIDLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("%s",Carrierid);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("BEGIN");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMNAME %s",_STR_PHYLOCATION);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMTYPE STRING");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMVAL %s",CarrierEntered);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
   /*.................................................................... */
   /* Send command and get response...                                   */ 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (_TLK_TGTPRI_SETLOTLOCATION);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (_TLK_TGTPRI_SETLOTLOCATION);
   
   /*.....................................................................*/
   /* Make sure that there was successful completion...                   */
   
   if (TRC_ChkAlarm()) 
      PRS_PTP_ChkStatus ();
   
   if (!TRC_ChkAlarm())
   {
      *ptpstatus= 0;
      strcpy(ErrorMsg,TRC_ErrorMsg());
      
      /*******************************************************************************
      **   Start Mod for catching failed setlotlocation if due to locked record
      **   in promis
      **/
      PRS_PTP_GetFirst (ErrM, _SIZ_GENFIELD); /*  Get Promis error token  */
      if (!strncmp(ErrM,_PRI_LOTLOCKED, strlen(_PRI_LOTLOCKED)))
      {
         retval = KeepFailedSetLocations( 1, Carrierid, Amhsequipid, Category, Number);
//         TRC_Send( LOG_ALIAS, _TRC_LVL_INFO,"%s:KFSL[%d][%s][%s][%s][%s]\n"
//            ,TRC_GetTime(), retval, Carrierid, Amhsequipid, Category, Number);
      }
      /**
      **   End Mod for setlotlocation
      **
      *******************************************************************************/
   }
   else
      DelFromFailedSetLocationsList( Carrierid);
   
   return (TRC_GetAlarm() );
}

/********************* End of function PRIMethodCarrierEntered ******************/


int PRIMethodCarrierRemoved(char *Carrierid,char *Amhsequipid,int *ptpstatus,char *ErrorMsg)
{
   char PromUsrId[_SIZ_USERID];
   char PromPasswd[_SIZ_PASSWORD];
   char CarrierRemoved[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=-1;
   
   GetPromUsrPwd(PromUsrId, PromPasswd);
   sprintf ( CarrierRemoved,"%s,Transit",Amhsequipid);
   
   if (TRC_ChkAlarm()) PRS_PTP_InitCommand (_PRS_PTP_UPDATEPARAM);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID %s", PromUsrId);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PWD %s", PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("LOTIDLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("%s",Carrierid);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("BEGIN");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMNAME %s",_STR_PHYLOCATION);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMVAL %s",CarrierRemoved);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
   /*.................................................................... */
   /* Send command and get response...                                   */ 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (_TLK_TGTPRI_CLEARLOTLOCATION);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (_TLK_TGTPRI_CLEARLOTLOCATION);
   
   /*.....................................................................*/
   /* Make sure that there was successful completion...                   */
   
   if (TRC_ChkAlarm()) PRS_PTP_ChkStatus ();
   if (!TRC_ChkAlarm())
   {
      *ptpstatus= 0;
      strcpy(ErrorMsg,TRC_ErrorMsg());
      
      /*******************************************************************************
      **  Start Mod for catching failed setlotlocation if due to locked record
      **  in promis
      **/
      PRS_PTP_GetFirst (ErrM, _SIZ_GENFIELD); /*  Get Promis error token  */
      if ( !strncmp(ErrM,_PRI_LOTLOCKED, strlen(_PRI_LOTLOCKED)))
      {
         retval = KeepFailedSetLocations( 2, Carrierid, Amhsequipid, "NULL", "NULL");
//         TRC_Send(LOG_ALIAS, _TRC_LVL_INFO,"%s:KFSL[%d][%s]\n",TRC_GetTime(), retval,Carrierid);
      }
      /**
      **   End Mod for setlotlocation
      **
      *******************************************************************************/
   }
   else
      DelFromFailedSetLocationsList( Carrierid);
   return (TRC_GetAlarm() );
}

int PRIMethodLocChanged(char *Carrierid,char *Amhsequipid,char *Category, char *Number,int *ptpstatus,char *ErrorMsg)
{
   char PromUsrId[_SIZ_USERID];
   char PromPasswd[_SIZ_PASSWORD];
   char LocChanged[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=-1;
   
   GetPromUsrPwd(PromUsrId, PromPasswd);
   sprintf ( LocChanged,"%s,%s%s",Amhsequipid,Category,Number);
   if (TRC_ChkAlarm()) PRS_PTP_InitCommand (_PRS_PTP_UPDATEPARAM);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID %s", PromUsrId);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PWD %s", PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("LOTIDLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("%s",Carrierid);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("BEGIN");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMNAME %s",_STR_PHYLOCATION);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMVAL %s",LocChanged);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
   /*.................................................................... */
   /* Send command and get response...                                   */ 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (_TLK_TGTPRI_LOCCHANGE);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (_TLK_TGTPRI_LOCCHANGE);
   
   /*.....................................................................*/
   /* Make sure that there was successful completion...                   */
   
   if (TRC_ChkAlarm())
      PRS_PTP_ChkStatus ();
   if (!TRC_ChkAlarm())
   {
      *ptpstatus= 0;
      strcpy(ErrorMsg,TRC_ErrorMsg());
      
      /*******************************************************************************
      **   Start Mod for catching failed setlotlocation if due to locked record
      **   in promis
      **/
      PRS_PTP_GetFirst (ErrM, _SIZ_GENFIELD); /*  Get Promis error token  */
      if (!strncmp(ErrM,_PRI_LOTLOCKED, strlen(_PRI_LOTLOCKED)))
      {
         retval = KeepFailedSetLocations( 3, Carrierid, Amhsequipid, Category, Number);
//         TRC_Send(LOG_ALIAS, _TRC_LVL_INFO,"%s: Saving failed setlotloc [%d][%s]\n",TRC_GetTime(), retval,Carrierid);
      }
      /**
      **   End Mod for setlotlocation
      **
      *******************************************************************************/
   }
   else
      DelFromFailedSetLocationsList( Carrierid);
   return (TRC_GetAlarm() );
}


/******************* Start of Move function **********************/
int PRIMethodMove(char *LotId, char *EqptId, char *UserId, char *OutPort, char *ErrorMsg)
{
   char cmd[_SIZ_GENFIELD]={0};
   int  ecd=0, statCode=0;
   
   TRC_ClrAlarm();
   
   PRS_VFI_InitCommand("MACH_CMD",TRANSNET_NAME);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CMDTYP,_SIZ_CMDTYPE, "MOVE");
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CARRIERID,_SIZ_CARRIERID,
      LotId);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_DESTID,_SIZ_DESTID, EqptId);
   /*************************************************************************************
   ***  Put Operator Id as part of move command
   ***/
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_USERID, _SIZ_USERID, UserId);
   /***
   ***  Put Operator Id as part of move command
   ************************************************************************************/
   
   /*
   | Only add _TOK_OUTPUTPORT if OutPort contains a value
   */
   if (strlen(OutPort) > 0 && strcmp(OutPort,"NONE"))
   {
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_OUTPUTPORT,_SIZ_OUTPUTPORT,OutPort);
   }   
   message.msgType = _TLK_TYPVFEI;
   
   //  TRC_Send(LOG_ALIAS, _TRC_LVL_DEBUG, "%s: %s\n",TRC_GetTime(), message.buffer);

   if (TLK_DMQ_ClientPut (hTransNetServer) != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
   if (TLK_DMQ_ClientGet (hTransNetServer) != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
      /*
      * Log failure to the logfile handle.
   */
   if ( PRS_VFI_ChkStatus() != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
   if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "CMD"));
   
   if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0)
      return (TRC_SetAlarm("MOVE", _RET_CMDWRONG, cmd, "CMD_ACK"));
   
   if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "ECD"));
   
   if (ecd != 0) 
      return (TRC_SetAlarm("MOVE", _RET_FAILURE, &ecd, "ECD NOT ZERO"));
   return (TRC_ChkAlarm());
}
/********************* End of Move function *****************/

/*************************************************************************/
int PRIMethodGetLotLoc(char *Carrierid,char *Amhsequipid,char *Location,int *ptpstatus,char *ErrorMsg)
{
   char PromUsrId[_SIZ_USERID];
   char PromPasswd[_SIZ_PASSWORD];
   int  ParmVal_List_Count,Search_Flag,i;
   char ParmName[_SIZ_PROMPARM];
   char ParmVal[_SIZ_PROMPARM];  
   char *tmpUserId;
   char TmpStr[_SIZ_GENFIELD];
   
   GetPromUsrPwd(PromUsrId, PromPasswd);
   
   if (TRC_ChkAlarm()) PRS_PTP_InitCommand (_PRS_PTP_LOTINFO);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID %s", PromUsrId);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PWD %s", PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("LOTID %s",Carrierid);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("FROM ACTL.PARAMETERS");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SHOW PARMNAME");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SHOW PARMVAL");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
   
   /*.................................................................... */
   /* Send command and get response...                                   */ 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (_TLK_TGTPRI_GETLOTLOCATION);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (_TLK_TGTPRI_GETLOTLOCATION);
   
   /*.....................................................................*/
   /* Make sure that there was successful completion...                   */
   
   if (TRC_ChkAlarm()) PRS_PTP_ChkStatus ();
   if (!TRC_ChkAlarm())
   {
      *ptpstatus= 0;
      strcpy(ErrorMsg,TRC_ErrorMsg());        
      return (TRC_GetAlarm() );
   }
   
   if (TRC_ChkAlarm()) PRS_PTP_GetFirst (&ParmVal_List_Count,_PRS_INT);
   Search_Flag = 0;
   
   strcpy(Amhsequipid,"");
   strcpy(Location,"");
   for (i=0;i< ParmVal_List_Count;i++)
   {
      if (TRC_ChkAlarm()) PRS_PTP_GetNext(ParmName,_SIZ_PROMPARM);
      if (TRC_ChkAlarm()) PRS_PTP_GetNext(ParmVal,_SIZ_PROMPARM);
      if (strcmp(ParmName,_STR_PHYLOCATION) == 0) 
      { Search_Flag = 1;
      break;
      }
   }
   
   if ( (Search_Flag==1)  && (strlen(ParmVal) > 0)  )
   {
      strcpy ( TmpStr,ParmVal);
      tmpUserId = strtok(TmpStr,",");
      if ( tmpUserId != (char*) NULL)
      {
         strcpy(Amhsequipid,tmpUserId);
         strcpy(Location,&ParmVal[strlen(Amhsequipid)+1]);
      }
   }   
   if ( strlen(Amhsequipid) == 0 || strlen(Location) == 0 )
      return TRC_SetAlarm("GetLotLocation",_RET_GENERIC,
      "Get lot location not successful" );
   return (TRC_GetAlarm() );
}
  

/******************* Start of Move function for non-vfei standard**********************/

int PRIMethodMoveLotLoc(char *Carrierid,char *Eqptid,char *Location,
                        char *UserID, char *Priority, int *pristatus,char *ErrorMsg)
{
   char cmd[_SIZ_GENFIELD];
   int  ecd;
   
   TRC_ClrAlarm();
   
   PRS_VFI_InitCommand("MACH_CMD",TRANSNET_NAME);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CMDTYP,_SIZ_CMDTYPE, "MOVE");
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CARRIERID,_SIZ_CARRIERID,
      Carrierid);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_DESTID,_SIZ_DESTID, Eqptid);
   /*
   ************************************************************************************
   ***  Put Operator Id as part of move command
   ***/
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_USERID, _SIZ_USERID, UserID);
   /***
   ***  Put Operator Id as part of move command
   ************************************************************************************/
   
   /*
   | if Location is empty ||, it is a store cmd.  put OUTPUTPORT=""
   | if Location is "NONE", it is a store cmd. put OUTPUTPORT=""
   | If Location is passed as a string, pass it on as _TOK_OUTPUTPORT = <Location>
   */
   
   if(TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_OUTPUTPORT,_SIZ_OUTPUTPORT,Location);
   
   message.msgType = _TLK_TYPVFEI;
   
   if (TLK_DMQ_ClientPut (hTransNetServer) != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
   if (TLK_DMQ_ClientGet (hTransNetServer) != _RET_SUCCESS) 
   {
      *pristatus=4;
      strcpy(ErrorMsg,"Transnet timeout ");
      return (TRC_GetAlarm());
   }
   
   /*
   * Log failure to the logfile handle.
   */
   if (PRS_VFI_ChkStatus() != _RET_SUCCESS) 
   {
      TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
         TRC_GetTime(), _RET_FAILURE, "MOVE CMD FAILED AT VFEI LEVEL");
      return (TRC_GetAlarm());
   }
   
   if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "CMD"));
   
   if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0) 
      return (TRC_SetAlarm("MOVE", _RET_CMDWRONG, cmd, "CMD_ACK"));
   
   if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "ECD"));
   
   if (ecd != 0) 
   {
      *pristatus=ecd;
      PRS_VFI_GetToken("ETX",_SIZ_GENFIELD,ErrorMsg);
   }
   return ( TRC_GetAlarm());
}

/*************************************************************************/
/********************* End of file Move function for NVFEI standard*****************/
int PRIMethodShutDown(char *ErrorMsg)
{
   char cmd[_SIZ_GENFIELD];
   int  ecd;
   
   TRC_ClrAlarm();
   strcpy(ErrorMsg,"");
   
   PRS_VFI_InitCommand("MACH_CMD",TRANSNET_NAME);
   if (TRC_ChkAlarm())
      PRS_VFI_PutToken (_TOK_CMDTYP,_SIZ_CMDTYPE, "SHUTDOWN");
   message.msgType = _TLK_TYPVFEI;
   
   if (TLK_DMQ_ClientPut (hTransNetServer) != _RET_SUCCESS) 
   {
      TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
         TRC_GetTime(), _RET_FAILURE, "FAILED TO SEND SHUTDOWN CMD TO TRANSNET");
      return( TRC_SetAlarm("SHUTDOWN ", _RET_TRANSNET_INIT_FAIL));
   }
   
   if (TLK_DMQ_ClientGet (hTransNetServer) != _RET_SUCCESS) 
   {
      TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
         TRC_GetTime(), _RET_FAILURE, "FAILED TO RECEIVE REPLY FROM TRANSNET");
      return (TRC_SetAlarm("SHUTDOWN  ", _RET_TRANSNET_INIT_FAIL));
   }
   
   /*
   * Log failure to the logfile handle.
   */
   if (PRS_VFI_ChkStatus() != _RET_SUCCESS) 
   {
      TRC_Send( LOG_ALIAS, _TRC_LVL_WARN, "%s: Status = %d\t%s\n",
         TRC_GetTime(), _RET_FAILURE, "SHUTDOWN CMD FAILED AT VFEI LEVEL");
   }
   
   if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("SHUTDOWN", _RET_FLDMISSING, "CMD"));
   
   if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0) 
      return (TRC_SetAlarm("SHUTDOWN", _RET_CMDWRONG, cmd, "CMD_ACK"));
   
   if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("SHUTDOWN", _RET_FLDMISSING, "ECD"));
   if (ecd != 0) 
   {
      PRS_VFI_GetToken("ETX",_SIZ_GENFIELD,ErrorMsg);
      return (TRC_SetAlarm("SHUTDOWN", _RET_FAILURE, &ecd, "ECD NOT ZERO"));
   }       
   return (TRC_GetAlarm());
}

/******************* Start of GetPromUsrPwd            **********************/

int GetPromUsrPwd(char *UserId, char *Password)
{
   /*** Get the UserId ***/
   char *Temp;
   strcpy ( UserId,"PRISRV");
   strcpy (Password,"NONE");
   return 0;
   if ((Temp = (char *) getenv("PROMISUSERID")) == NULL)
   {
      TRC_SetAlarm("GetPromUsrPwd",_RET_GENERIC,
         "EnvironMent Variable for Promis UserId not Defined");
      //        fprintf(stderr,"Failed - promis_userid is %s \n",Temp);
      return TRC_GetAlarm();
   }
   
   //  printf("Passed - promis userid is %s \n",Temp);
   strcpy(UserId,Temp);
   strcpy(UserId,"PROMIQA");
   
   if ((Temp = (char *) getenv("PROMISPASSWD")) == NULL)
   {
      TRC_SetAlarm("GetPromUsrPwd",_RET_GENERIC,
         "EnvironMent Variable for Promis Password not Defined");
      //        fprintf(stderr,"Failed - promis_password is %s \n",Temp);
      return TRC_GetAlarm();
   }
   
   //  printf("Passed - promis_password is %s \n",Temp);
   strcpy(Password,Temp);
   strcpy(Password,"STARTREK");
   
   return _RET_SUCCESS;
}
/******************* End of GetPromUsrPwd **********************/


/*.. begin function PRIMethodToshMcsLocChange ...............................................................*/
int PRIMethodToshMcsLocChange(int arrcount, char lotid[][_SIZ_CARRIERID], char lotlocation[][_SIZ_LOCATIONID])
{
   char PromUsrId[_SIZ_USERID];
   char PromPasswd[_SIZ_PASSWORD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=-1, i;

   GetPromUsrPwd(PromUsrId, PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_InitCommand (_PRS_PTP_MULTIUPDATE);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID %s", PromUsrId);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PWD %s", PromPasswd);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("BIGLOTIDLIST %d", arrcount);
   for (i=0;i<arrcount;i++)
   {
      if (TRC_ChkAlarm())
         PRS_PTP_PutToken ("%s",lotid[i]);
   }

   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("MULTIPARMLIST 1");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("BEGIN");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMNAME %s",_STR_PHYLOCATION);
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMTYPE STRING");
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("PARMVALLIST %d", arrcount);
   for (i=0;i<arrcount;i++)
   {
      if (TRC_ChkAlarm())
         PRS_PTP_PutToken ("%s",lotlocation[i]);
   }
   if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
   /*.................................................................... */
   /* Send command and get response...                                   */ 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (_TLK_TGTPRI_SETLOTLOCATION);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (_TLK_TGTPRI_SETLOTLOCATION);
   
   /*.....................................................................*/
   /* Make sure that there was successful completion...                   */
   
   if (TRC_ChkAlarm())
      PRS_PTP_ChkStatus();
   return (TRC_GetAlarm() );
}
/*.. end func .....................................................................*/

/*...Function to get location for SMP lots in toshiba stocker...*/
/*
1. Get all active lots from SMP (RPM Query)
2. Send to GATEWAY->FAB3PRI->MCS->FAB3PRI->GATEWAY->SMP
3. Update lot loc for valid lots( LOT PARM MULTI UPDATE)
4. This is done automatically as the response comes as an event
     from FAB3 side
*/
/*
|
|    This functionality moved to TICKSRV
|    The management of the reply is kept here itself.
|
int PRIMethodToshLotLoc(void)
{
   char LotBuckets[_MAX_CARRIER][_SIZ_LOTID];
   char LotList[_MAX_300][_SIZ_LOTID], tstr[_SIZ_LOTID];
   int totLots, ictr, tctr, count, retval;

   TRC_ClrAlarm();
   PRS_PTP_InitCommand (_PROMIS_CMD_RPMLOTINFO);
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID EVSTSRV");
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("ACTIVE");
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("WHERE ACTLFLD('locationtype') = 'W'");
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SORT LOTID");
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SHOW LOTID");
        if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
  /*.................................................................... * /
   /* Send command and get response...                                   * / 
   if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (hRPMserver);
   if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (hRPMserver);

   /*.....................................................................* /
   /* Make sure that there was successful completion...                   * /
   PRS_PTP_ChkStatus ();
   if (!TRC_ChkAlarm())
      return -1; /*... indicating error happened ...* /

   /*... get the total number of lots retrieved ...* /
   PRS_PTP_GetFirst(&totLots, _PRS_INT);
printf("\nTot Bucket lots %d\n", totLots);
    if(!TRC_ChkAlarm())
      return -1; /*... indicating error happened ...* /

   if(totLots <1)
      return -1; /*... indicating error happened ...* /
      
   /*...Make buckets...* /
   memset(LotBuckets, 0, sizeof(LotBuckets));

   for(tctr=0,ictr=1,count=0;tctr<totLots&&(TRC_ChkAlarm());++tctr,++ictr)
   {
         PRS_PTP_GetNext(tstr, _SIZ_LOTID);
         if(count==0||ictr==_MAX_300)
         {
              rTrim(tstr);
              strcpy(LotBuckets[count++],tstr);
              ictr=1;
         }
   }
   /*...Pick the last lot into the bucket too...* /
   rTrim(tstr);
   strcpy(LotBuckets[count],tstr);   
printf("Buckets created. Total queries=%d\n", count);
   
   /*......
              Query each bucket and send to Toshiba
                                                       ...................* /
   while(count)
   {                                                       
           TRC_ClrAlarm();
           PRS_PTP_InitCommand (_PROMIS_CMD_RPMLOTINFO);
           if (TRC_ChkAlarm()) PRS_PTP_PutToken ("USERID EVSTSRV");
           if (TRC_ChkAlarm()) PRS_PTP_PutToken ("ACTIVE");
           if (TRC_ChkAlarm()) 
               PRS_PTP_PutToken (
                  "WHERE ACTLFLD('locationtype') = 'W' AND LOTID BETWEEN '%s' AND '%s'"
                , LotBuckets[count-1], LotBuckets[count]);
           if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SORT LOTID");
           if (TRC_ChkAlarm()) PRS_PTP_PutToken ("SHOW LOTID");
           if (TRC_ChkAlarm()) PRS_PTP_PutToken ("END");
           /*.................................................................... * /
           /* Send command and get response...                                    * / 
           if (TRC_ChkAlarm()) TLK_DMQ_ClientPut (hRPMserver);
           if (TRC_ChkAlarm()) TLK_DMQ_ClientGet (hRPMserver);
        
           /*.....................................................................* /
           /* Make sure that there was successful completion...                   * /
           PRS_PTP_ChkStatus ();
           if (!TRC_ChkAlarm())
              return -1; /*... indicating error happened ...* /
        
           /*... get the total number of lots retrieved ...* /
           PRS_PTP_GetFirst(&totLots, _PRS_INT);
           printf("%d.Tot lots %d\n", count, totLots);
           if(!TRC_ChkAlarm())
              return -1; /*... indicating error happened ...* /
        
           if(totLots <1)
              return -1; /*... indicating error happened ...* /
                 
           for(tctr=0; tctr<totLots&&(TRC_ChkAlarm());)
           {
              memset(LotList, 0, sizeof(LotList));
              /*... load list with lot ids ...* /
              for(ictr=0;ictr<_MAX_300&&(tctr<totLots)&&(TRC_ChkAlarm());++ictr,++tctr)
              {
                 PRS_PTP_GetNext(LotList[ictr], _SIZ_LOTID);
                 rTrim(LotList[ictr]);
              }
              printf("Current tctr=%d, ictr=%d.\n", tctr,ictr);
              /*...we come out if _MAX_CARRIER is reached or end of parse buffer...* /
              /*...in either case send the message ...* /
              retval=FAB3CreateSendF3Event( "MCS_QRY_LOT_LIST", ictr , LotList, NULL, NULL);
           }
           count--;
   }
   return tctr;
}
|
|
*/
/*.. end func .....................................................................*/

