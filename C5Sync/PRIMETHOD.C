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

   char CarrierEntered[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=0;
   
   //GetPromUsrPwd(PromUsrId, PromPasswd);
   sprintf ( CarrierEntered,"%s,%s%s",Amhsequipid,Category,Number);
   fprintf(stderr, "CarrierEntered:update global section for Lot:%s and value: %s\n",Carrierid, CarrierEntered);
   size_t lotId_len = strlen(Carrierid);
   size_t mcsLoc_len = strlen(CarrierEntered);//EQP_ID_LEN + LOC_ID_LEN + 1;

//   TRC_Send( LOG_ALIAS, _TRC_LVL_INFO,
//      "%s:Exec carrier entered\n [%s][%s][%s][%s][%s]\n",
//      TRC_GetTime(), Carrierid, Amhsequipid, Category, Number,CarrierEntered);
   updateLotLocInPROMIS(Carrierid, CarrierEntered, &lotId_len, &mcsLoc_len);

   return (TRC_GetAlarm() );
}

/********************* End of function PRIMethodCarrierEntered ******************/


int PRIMethodCarrierRemoved(char *Carrierid,char *Amhsequipid,int *ptpstatus,char *ErrorMsg)
{
   char CarrierRemoved[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=-1;
   

   sprintf ( CarrierRemoved,"%s,Transit",Amhsequipid);
   fprintf(stderr, "CarrierRemoved:update global section for Lot:%s and value: %s\n",Carrierid, CarrierRemoved);

   size_t lotId_len = strlen(Carrierid);
   size_t mcsLoc_len = strlen(CarrierRemoved);//EQP_ID_LEN + LOC_ID_LEN + 1;
   updateLotLocInPROMIS(Carrierid, CarrierRemoved, &lotId_len, &mcsLoc_len);

   return (TRC_GetAlarm() );
}

int PRIMethodLocChanged(char *Carrierid,char *Amhsequipid,char *Category, char *Number,int *ptpstatus,char *ErrorMsg)
{
   char LocChanged[_SIZ_GENFIELD];
   char ErrM[_SIZ_GENFIELD]={0};
   int retval=-1;

   sprintf ( LocChanged,"%s,%s%s",Amhsequipid,Category,Number);
   fprintf(stderr, "LocChanged:update global section for Lot:%s and value: %s\n",Carrierid, LocChanged);

   size_t lotId_len = strlen(Carrierid);
   size_t mcsLoc_len = strlen(LocChanged);//EQP_ID_LEN + LOC_ID_LEN + 1;
   updateLotLocInPROMIS(Carrierid, LocChanged, &lotId_len, &mcsLoc_len);

   return (TRC_GetAlarm() );
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


