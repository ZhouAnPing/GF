/**********************************************************************************************
File : PRILoc.c
---------------
File added to the existing PRISvr to talk care of additional functionality
1.  Differentiate between SMP Locations and FAB3 Locations
2.  Store and re-process failed SET_LOT_LOCATION commands
3.  Verbosity 3 levels
4.  send CMD_ACK for HEARTBEAT queries.

*********************************************************************************************/

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*...CIMLIB include files...*/
#ifdef WIN32
    #include "cimdefs.h"
    #include "ordarray.h"
    #include "trcutil.h"
    #include "cimtokens.h"
    #include "parsutl.h"
    #include "talkutl.h"
#else
    #include "ssp_cimlib:cimdefs.h"
    #include "ssp_cimlib:ordarray.h"
    #include "ssp_cimlib:trcutil.h"
    #include "ssp_cimlib:cimtokens.h"
    #include "ssp_cimlib:parsutl.h"
    #include "ssp_cimlib:talkutl.h"
#endif

/*...MHServ include files...*/
#include "prisrv.h"


/*********************************************************************************************
Function              : KeepFailedSetLocations( int Type, char *Carrierid,char *Amhsequipid,char *Category,
char *Number )
Parameters    : int Type, char *Carrierid,char *Amhsequipid,char *Category, char *Number
Returns               : int 

  Functionality : This function is called for each lot that fails the set location command. 
  The lots are strungup in a linked list to be executed the next time.
  Added Type to differentiate between CE, CR and LC.
  
    Revision History:
    ~~~~~~~~~~~~~~~~
    Name  Date    Change done
    ----  ------- -------------------------------------------------------------------------------
    Ivan  000926  Started coding this function to add to existing PRISvr.
    
*********************************************************************************************/
int KeepFailedSetLocations( int Type, char *Carrierid,char *Amhsequipid,char *Category, char *Number)
{
   struct FAILLOTstruct *new=NULL;
   
//   TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s: Adding [%s,%s,%s,%s] to list.\n"
//      , TRC_GetTime(), Carrierid, Amhsequipid, Category, Number);
   
   if( !( new=(struct FAILLOTstruct*) malloc( sizeof(struct FAILLOTstruct)) ) )
   {
      TRC_Send(LOG_ALIAS, _TRC_LVL_DEBUG, 
         "\n%s: Cannot allocate [%d] bytes of memory for CarrierId[%s].\n",
         TRC_GetTime(), sizeof(struct FAILLOTstruct), Carrierid);
      return -1;  /* return null to indicate error */
   }
   new->next=NULL;
   strncpy(new->Carrierid, Carrierid, _SIZ_CARRIERID);
   strcpy(new->Amhsequipid, Amhsequipid);
   strcpy(new->Category, Category);
   strcpy(new->Number, Number);
   new->Type=Type;
   
   //        TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s: Adding [%s,%s,%s,%s] to list. "
   //                  , TRC_GetTime(), new->Carrierid, new->Amhsequipid, new->Category
   //                  , new->Number);
   
   if (FAILLOTstart) /* Other lots exist */
      AddToFailedSetLocations(new);
   else /* first lot being added */
   {
      FAILLOTstart=new;
//      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "Ok.\n");
   }
   
   //      Debug_list(1); /* change to 0 to stop printing but display debug message */
   
   return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : AddToFailedSetLocations(new)
Parameters    : struct FAILLOTstruct *
Returns               : int 

  Functionality : This function is called for each lot that fails the set location command. 
  It puts them into the list in alphanumeric order.
  
    **** Take care as the first node is allocated directly in the calling 
    **** function and not here. 
    
      Revision History:
      ~~~~~~~~~~~~~~~~
      Name  Date    Change done
      ----  ------- -------------------------------------------------------------------------------
      Ivan  000926  Started coding this function to add to existing PRISvr.
      
*********************************************************************************************/
int AddToFailedSetLocations( struct FAILLOTstruct *new )
{
   struct FAILLOTstruct *prev=NULL, *curr=NULL, *pptr=NULL;
   int retval=0, lcount=0;
   
   curr=FAILLOTstart;
   prev=curr;
   for(prev=curr;(curr) && (retval=strcmp( new->Carrierid, curr->Carrierid))>0 ;curr=curr->next)
      prev=curr;
   
   if(curr) /* insert before end of list */
   {
      if (retval) /* if retval !=0, means it does not exist in list, so add it */
      {
         if(curr==FAILLOTstart)
         {
            /* insert at begining of list */
            new->next=FAILLOTstart;
            FAILLOTstart=new;
         }
         else
         {
            /* insert into middle */
            prev->next=new;
            new->next=curr;
         }
      }
      else /* retval = 0, so update existing node */
      {
         // strcpy(curr->Carrierid,new->Carrierid); <-- should we do this?
         strcpy(curr->Amhsequipid,new->Amhsequipid);
         strcpy(curr->Category,new->Category);
         strcpy(curr->Number,new->Number);
         curr->Type=new->Type;
         free(new);
//         TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "Lot exists, Updated, Ok.\n");
         return 0; /* Exit here to avoid double message in log file */
      }
   }
   else /* insert at end of list */
      prev->next=new;
//   TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "Added, Ok.\n");
   return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              :       int CallExecFailedLots(void)

  Parameters    :       none
  
    Returns               : int
    
      Functionality :       
      This function is called to re-run the failed lots 
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int CallExecFailedLots(void)
{
   int RetVAL=-1, total=0;
   
//   Debug_list(1);
   RetVAL=ExecFailedSetLocations(&total);
   TRC_Send(LOG_ALIAS,  _TRC_LVL_DEBUG,"%s: Execed %d of %d successfully.\n"
      , TRC_GetTime(), RetVAL, total);
//   Debug_list(1);
   return RetVAL;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : ExecFailedSetLocations()
Parameters    : None
Returns               : int

  Functionality : This function is called for each lot that fails the set location command. 
  The lots are strungup in a linked list to be executed the next time.
  
    Revision History:
    ~~~~~~~~~~~~~~~~
    Name  Date    Change done
    ----  ------- -------------------------------------------------------------------------------
    Ivan  000926  Started coding this function to add to existing PRISvr.
    
*********************************************************************************************/
int ExecFailedSetLocations(int *totalrecs)
{
   char ErrorMsg[_SIZ_GENFIELD] = {0};
   int ptpstatus;
   struct FAILLOTstruct *prev=NULL, *curr=NULL, *tptr=NULL;
   int retval=0, listcount=0,successcount=0;
   
//   TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG,
//      "%s: ReExecuting SETLOTLOCATION for failed locations.\n", TRC_GetTime());
   //      fprintf(stderr, "%s: ReExecuting SETLOTLOCATION for failed locations.\n", TRC_GetTime());
   
   curr=FAILLOTstart;
   prev=curr;
   while(curr)
   {
      ++listcount;
      ErrorMsg[0]='\0';
      TRC_ClrAlarm();
//      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, 
//         "%s:[%s][%s][%s][%s][%d].\n", TRC_GetTime(),
//         curr->Carrierid, curr->Amhsequipid, curr->Category, curr->Number,curr->Type);
      switch (curr->Type)
      {
      case 1: retval=PRIMethodCarrierEntered(curr->Carrierid, curr->Amhsequipid, curr->Category, 
                 curr->Number,&ptpstatus,ErrorMsg);
         break;
      case 2: retval =PRIMethodCarrierRemoved(curr->Carrierid, curr->Amhsequipid,&ptpstatus,ErrorMsg);
         break;
      case 3: retval =PRIMethodLocChanged(curr->Carrierid, curr->Amhsequipid, curr->Category, 
                 curr->Number,&ptpstatus,ErrorMsg);
      }
      
      /*              fprintf(stderr, "%s:[R.%s][C.%s][%s][%s][%s][%d][%10.10s].\n", TRC_GetTime(),
      (TRC_ChkAlarm()? "Succ":"Fail"), curr->Carrierid, curr->Amhsequipid, 
      curr->Category, curr->Number,ptpstatus,ErrorMsg);
      */
      if (TRC_ChkAlarm())
      {
         ++successcount;
         tptr=curr->next;
         if(curr==FAILLOTstart)
         {
            FAILLOTstart=curr->next;
            free(curr);
         }
         else
         {
            prev->next=curr->next;
            free(curr);
         }
         curr=tptr;
      }
      else
      {
         prev=curr;
         curr=curr->next;
      }
   }
   /* Return values for total recs and exec-ed recs */
   *totalrecs=listcount;
   return successcount;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : FreeFailedSetLocationsMem()
Parameters    : NONE - FAILLOTstart could be passed, but its a global pointer therefore there is 
no need to pass it as a parameter
Returns               : int

  Functionality : This function is called to free the FAILLOTS list in case of abnormal shutdown.
  
    Revision History:
    ~~~~~~~~~~~~~~~~
    Name  Date    Change done
    ----  ------- -------------------------------------------------------------------------------
    Ivan  000926  Started coding this function to add to existing PRISvr.
    
*********************************************************************************************/
int FreeFailedSetLocationsMem()
{
   struct FAILLOTstruct *curr=NULL, *prev=NULL;
   int lcount=0;
   TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s: Pending CarrierIds for Set Location...\n",TRC_GetTime());
   
   curr=FAILLOTstart;
   while(curr)
   {
      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s: %3d. [%s][%s][%s][%s][%d]\n",
         TRC_GetTime(), ++lcount, curr->Carrierid, curr->Amhsequipid, curr->Category, curr->Number,
         curr->Type);
      prev=curr;
      curr=curr->next;
      free(prev);
   }
   FAILLOTstart=NULL;
   return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : Debug_list( int )
Parameters    : int 
0 - dont print list
1 - print list
Returns               : int

  Functionality : This function is called to print the list for debugging purpose.
  
    Revision History:
    ~~~~~~~~~~~~~~~~
    Name  Date    Change done
    ----  ------- -------------------------------------------------------------------------------
    Ivan  000926  Started coding this function to add to existing PRISvr.
    
*********************************************************************************************/
int Debug_list( int direction )
{
   struct FAILLOTstruct *pptr=NULL;
   int lcount=0;
   if (direction)
   {
      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s:*********   LIST CONTENTS ARE   *********\n",TRC_GetTime());
      for(pptr=FAILLOTstart;pptr;pptr=pptr->next)
      {
         TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "\t%3d: [%s][%s][%s][%s][%d]\n",++lcount, pptr->Carrierid,
            pptr->Amhsequipid,pptr->Category, pptr->Number, pptr->Type);
      }
      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s:*********   END LIST   [%d]item%s *********\n\n",
         TRC_GetTime(),lcount,(lcount==1?" ":"s"));
   }
   else
      TRC_Send( LOG_ALIAS, _TRC_LVL_DEBUG, "%s:Debug_list setting is off.\n",TRC_GetTime());
   return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              :       int DelFromFailedSetLocationsList( char Type, char *CarrierId )

  Parameters    : 
  char *stocker  - stocker for which CarrierIds and Locations  to be sent dd
  array
  
    Returns               : int
    True
    False
    
      Functionality :       This function is called to delete failedsetlocation CarrierIds.
      Only called from place where there is a successful setlocation done.
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/

int DelFromFailedSetLocationsList( char *CarrierId )
{
   struct FAILLOTstruct *prev=NULL, *curr=NULL, *pptr=NULL;
   int retval=0, lcount=0;
   
   curr=FAILLOTstart;
   
   for(prev=curr;(curr) && strcmp( CarrierId, curr->Carrierid);curr=curr->next)
      prev=curr;
   
   if(!curr)
      return -1;
   
   if(curr==FAILLOTstart)
   {
      FAILLOTstart=curr->next;
      free(curr);
   }
   else
   {
      prev->next=curr->next;
      free(curr);
   }
   return 0;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int FAB3EventSYNC()
Parameters    : none 

  Returns               : int
  0 - Success
  1 - Failure
  
    Functionality : This function is called to sync.
    
      Revision History:
      ~~~~~~~~~~~~~~~~
      Name  Date    Change done
      ----  ------- -------------------------------------------------------------------------------
      Ivan  000926  Started coding this function to add to existing PRISvr.
      
*********************************************************************************************/
int FAB3EventSYNC()
{
   int retval=0;
   
   TRC_ClrAlarm ();
   PRS_VFI_InitEvent (_TOK_FAB3SYNC, _TOK_FAB3SYNCMC);
   if( TRC_ChkAlarm()) 
      PRS_VFI_PutToken (_TOK_SERVER,_SIZ_SERVER, _TOK_FAB3SYNCMC);
   
   /* Sends command and get response */
   message.msgType = _TLK_TYPVFEI;
   retval=TLK_DMQ_ClientPut (hGatewayServer);
   //    TRC_Send( LOG_ALIAS, _TRC_LVL_STATE, "%s: FAB3EventSYNC Status = %d\tAlarmCode[%d]AlarmMsg[%s]\n",
   //              TRC_GetTime(), retval, TRC_GetAlarm(), TRC_ErrorMsg());
   return (TRC_ChkAlarm());
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int IsSMPLoc(char *)
Parameters    : char * - location name to be checked

  Returns               : int
  True
  False
  
    Functionality : This function is called to  differentiate between lots in FAB3 and SMP 
    locations
    
      Revision History:
      ~~~~~~~~~~~~~~~~
      Name  Date    Change done
      ----  ------- -------------------------------------------------------------------------------
      Ivan  000926  Started coding this function to add to existing PRISvr.
      
*********************************************************************************************/
int IsSMPLoc(char *loc)
{
   int retval=-1;
   struct iList *curr=NULL;
   
   /***  Loop till end of list or retval = 0 (false)
   ***/
   /*... match==>retval is zero == fab3 loc
       , nomatch/end of list==>reval is non-zero == smp loc ...*/
   for(curr=fab3LocationStart;(curr) && retval; curr=curr->next)
      retval=strcmp(curr->iName,loc);
   return (retval);
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int FAB3MethodCarrierEntered( char *Carrierid, char *Amhsequipid,
char *Category, char *Number, int *ptpstatus, char *ErrorMsg)
Parameters    :       Carrierid
Amhsequipid
Category
Number
ptpstatus
ErrorMsg

  Returns               : int
  _RET_SUCCESS
  
    Functionality : This function is called to send CarrierEntered for fab3 lots to FAB3 
    through the GATEWAY 
    
      Revision History:
      ~~~~~~~~~~~~~~~~
      Name  Date    Change done
      ----  ------- -------------------------------------------------------------------------------
      Ivan  000926  Started coding this function to add to existing PRISvr.
      
*********************************************************************************************/
int FAB3MethodCarrierEntered( char *Carrierid, char *Amhsequipid, char *Category, char *Number,
                             int *ptpstatus, char *ErrorMsg)
{
   int retval=0;
   char tmpcarrierid[2][_SIZ_CARRIERID]
      , tmplocation[2][_SIZ_GENFIELD]
      , tmpequipment[2][_SIZ_GENFIELD]
      , s1[4]={0}, s2[4]={0}, s3[4]={0}, s4[4]={0};
   
   TRC_ClrAlarm ();
   //return (TRC_ChkAlarm()); //skip as gateway is not there 010611 ivan
   
   /***************************************************************************
   ***   For this REPLY we use the generic function FAB3CreateSendF3Event that  
   ***   builds the event to return thru GATEWAY
   ***
   ***/
   
   strcpy(tmpcarrierid[0],Carrierid);
   tmpcarrierid[1][0]='\0';
   
   strcpy(tmpequipment[0],Amhsequipid);
   tmpequipment[1][0]='\0';
   
   // Check for location build 
   // /* changes requested Ivan 552837*/  
   // sprintf ( tmplocation[0],"%s,%s%s",Amhsequipid,Category,Number);
   if ( strcmp(Category,"SHELF")==0)
   {
      if(strlen(Number)>7)
      {
         sscanf(Number,"%3s%2s%1s%3s%1s%3s",s4,s1,s4,s2,s4,s3);
         sprintf(Number,"%s%s%s",s1,s2,s3);
      }
   }          
   strcpy(Category,"");
   
   sprintf ( tmplocation[0],"%s%s", Category, Number);
   tmplocation[1][0]='\0';
   
   retval=FAB3CreateSendF3Event( "CARRIER_ENTERED", 1, tmpcarrierid, tmplocation, tmpequipment);
   return retval;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int FAB3MethodCarrierRemoved( char *Carrierid, char *Amhsequipid, 
int *ptpstatus, char *ErrorMsg)

  Parameters    :       Carrierid
  Amhsequipid
  ptpstatus
  ErrorMsg
  
    Returns               : int
    _RET_SUCCESS
    
      Functionality : This function is called to send CarrierRemoved for fab3 lots to FAB3 
      through the GATEWAY 
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int FAB3MethodCarrierRemoved( char *Carrierid, char *Amhsequipid, int *ptpstatus, char *ErrorMsg)
{
   int retval=0;
   char tmpcarrierid[2][_SIZ_CARRIERID]
      , tmplocation[2][_SIZ_GENFIELD]
      , tmpequipment[2][_SIZ_GENFIELD];
   
   TRC_ClrAlarm ();
   
   /***************************************************************************
   ***   For this REPLY we use the generic function FAB3CreateSendF3Event that  
   ***   builds the event to return thru GATEWAY
   ***
   ***/
   strcpy(tmpcarrierid[0],Carrierid);
   tmpcarrierid[1][0]='\0';
   
   strcpy(tmpequipment[0],Amhsequipid);
   tmpequipment[1][0]='\0';
   
   // Check for location build
   strcpy(tmplocation[0],"Transit");
   tmplocation[1][0]='\0';
   
   retval=FAB3CreateSendF3Event( "CARRIER_REMOVED", 1, tmpcarrierid, tmplocation, tmpequipment);
   //        if(!retval)
   //                TRC_Send(LOG_ALIAS, _TRC_LVL_STATE, 
   //                         "%s: FAB3MethodCarrierRemoved Status = %d\tAlarmCode[%d]AlarmMsg[%s]\n",
   //                               TRC_GetTime(), retval, TRC_GetAlarm(), TRC_ErrorMsg());
   return retval;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int FAB3MethodLocChanged( char *Carrierid, char *Amhsequipid, char *Category,
char *Number, int *ptpstatus, char *ErrorMsg)

  Parameters    :       Carrierid
  Amhsequipid
  Category
  Number
  ptpstatus
  ErrorMsg
  
    Returns               : int
    _RET_SUCCESS
    
      Functionality : This function is called to send LocChanged  for fab3 lots to FAB3 
      through the GATEWAY 
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int FAB3MethodLocChanged( char *Carrierid, char *Amhsequipid, char *Category, char *Number,
                         int *ptpstatus, char *ErrorMsg)
{
   int retval=0;
   char tmpcarrierid[2][_SIZ_CARRIERID] ={0}
   , tmpequipment[2][_SIZ_GENFIELD] ={0}
   , tmplocation[2][_SIZ_GENFIELD] ={0}
   , s1[4]={0}, s2[4]={0}, s3[4]={0}, s4[4]={0};
   
   TRC_ClrAlarm ();
   
   /***************************************************************************
   ***   For this REPLY we use the generic function FAB3CreateSendF3Event that  
   ***   builds the event to return thru GATEWAY
   ***
   ***/
   
   
   strcpy(tmpcarrierid[0],Carrierid);
   tmpcarrierid[1][0]='\0';
   
   strcpy(tmpequipment[0],Amhsequipid);
   tmpequipment[1][0]='\0';
   
   // Check for location build 
   // /* changes requested Ivan 551973 */  
   // sprintf ( tmplocation[0],"%s,%s%s",Amhsequipid,Category,Number);
   if ( strcmp(Category,"SHELF")==0)
   {
      if(strlen(Number)>7)
      {
         sscanf(Number,"%3s%2s%1s%3s%1s%3s",s4,s1,s4,s2,s4,s3);
         sprintf(Number,"%s%s%s",s1,s2,s3);
      }
   }
   strcpy(Category,"");
   sprintf ( tmplocation[0],"%s%s", Category, Number);
   
   tmplocation[1][0]='\0';
   
   retval=FAB3CreateSendF3Event( "LOC_CHANGED", 1, tmpcarrierid, tmplocation,tmpequipment);
   return retval;
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function              : int FAB3CreateSendF3Event( char *event, int arrcount, 
char **carrierid, char **location, char **equipment)

  Parameters    : int arrcount - count
  char **carrierid - carrier ids to be sent dd array
  char **location  - PROMISlocations  to be sent dd array
  
    Returns       : int
    True
    False
    
      Functionality : 
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int FAB3CreateSendF3Event( char *event, int arrcount, char carrierid[][_SIZ_CARRIERID]
                          , char location[][_SIZ_GENFIELD], char equipment[][_SIZ_GENFIELD])
{
   int retval=-1;
   
   if(arrcount==0)
      return(TRC_ChkAlarm());
   PRS_VFI_InitEvent (event, _TOK_FAB3SYNCMC);
   if (TRC_ChkAlarm()) 
      PRS_VFI_PutToken ("COUNT", _PRS_UINT, &arrcount);
   if (TRC_ChkAlarm()) 
      PRS_VFI_PutArray ("CARRIERID", _SIZ_CARRIERID, arrcount, carrierid);
   /*... add the location and equipment check as the same function is being used
     ......send MCS_QRY_LOT_LIST to qry smp lots in toshiba stocker               ...*/
   if(location)
      if (TRC_ChkAlarm()) 
         PRS_VFI_PutArray ("LOCATION", _SIZ_GENFIELD, arrcount, location);
   if(equipment)
      if (TRC_ChkAlarm()) 
         PRS_VFI_PutArray ("EQUIPMENTID", _SIZ_GENFIELD, arrcount, equipment);
   
   /* Send command */
   message.msgType = _TLK_TYPVFEI;
   retval=TLK_DMQ_ClientPut (hGatewayServer);
   
   //        TRC_Send(LOG_ALIAS, _TRC_LVL_DEBUG, "%s: FAB3CreateSendF3Event(%s) Status = %d\tAlarmCode[%d]AlarmMsg[%s]\n",
   //            TRC_GetTime(), event, retval, TRC_GetAlarm(), TRC_ErrorMsg());
   return (TRC_ChkAlarm());
}

/*********************************************************************************************
Function      : int FAB3ParseExecF3GetAllLotLoc(void)

  Parameters    : none
  
    Returns       : int
    
      Functionality : 
      This function is called to parse the event comming from the Gateway with
      names of stockers to list lot locs. It calls FAB3StockerDbList(char * stocker)
      which builds the event and returns the lots and locations to the caller.
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int FAB3ParseExecF3GetAllLotLoc(void)
{
/***********************************************************************
***  Get a list of stockers and call FAB3StockerDbList for each stocker
***  
   ***/
   char FAB3stocker[_MAX_CARRIER][_SIZ_STOCKERNAME]={0};
   int count=0, ctr=0;
   
   
   TRC_ClrAlarm();
   
   if( PRS_VFI_GetArray(_TOK_STOCKER,_MAX_CARRIER, _SIZ_STOCKERNAME,&count,FAB3stocker) != _RET_SUCCESS)
      return (TRC_SetAlarm("FAB3ParseExecF3GetAllLotLoc", _RET_FLDMISSING, _TOK_STOCKER));
   
   for(ctr=0;ctr!=count;ctr++)
   {
      if( !FAB3StockerDbList(FAB3stocker[ctr]))
         TRC_Send( LOG_ALIAS, _TRC_LVL_ERROR, "%s: StockerDbList Error[%d][%s]\n", 
         TRC_GetTime(), ctr, FAB3stocker[ctr]);
   }       
   return (TRC_ChkAlarm());
}
/***** END Function **************************************************************************/

/*********************************************************************************************
Function      :       int FAB3StockerDbList(char *stocker)

  Parameters    : 
  char *stocker  - stocker for which CarrierIds and Locations  are to be sent
  
    Returns       : int
    True
    False
    
      Functionality : This function is called to create return event containing CarrierIds and 
      Locations which are to be sent to FAB3 thru GATEWAY as per stocker passed 
      
        Revision History:
        ~~~~~~~~~~~~~~~~
        Name  Date    Change done
        ----  ------- -------------------------------------------------------------------------------
        Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
int FAB3StockerDbList(char *stocker)
{
   char vid[_MAX_VIDS][_SIZ_VID];
   char cmd[_SIZ_GENFIELD]={0};
   int pgnumber=0, pglength=0,count=0,tmppglength=0,i=0,FAB3count=0, retval=-1, siz=0,
      vidArraySize=0, tagidArraySize=0, tagdataArraySize=0, TokLocnamePos=0;
   char tmpamhsequipid[_MAX_CARRIER][_SIZ_AMHSEQUIPID];
   char tmpcarrierid[_MAX_CARRIER][_SIZ_CARRIERID];
   char tmpcategory[_MAX_CARRIER][_SIZ_CATEGORY];
   char tmpnumber[_MAX_CARRIER][_SIZ_TMPNUMBER];
   char dummynumber[_MAX_CARRIER][_SIZ_DUMMYNUMBER];
   char s1[4],s2[4],s3[4],s4[4];
   
   char PromUsrId[_SIZ_USERID]={0};
   char PromPasswd[_SIZ_PASSWORD]={0};
   char CarrierEntered[_SIZ_GENFIELD]={0};
   //        char tmplocname[_MAX_CARRIER][_SIZ_LOCNAME];
   char FAB3CarrierId[_MAX_CARRIER][_SIZ_CARRIERID];
   char FAB3LocationId[_MAX_CARRIER][_SIZ_GENFIELD];
   char FAB3equipment[_MAX_CARRIER][_SIZ_GENFIELD];
   char tagid[_NUM_TAGS][_SIZ_LOCNAME]; 
   char tagdata[_MAX_CARRIER * _NUM_TAGS][_SIZ_LOCNAME];
   char tmpSMP[_MAX_CARRIER + 1]={0};
   
   TRC_ClrAlarm();
   strcpy(vid[0],"AMHSEQUIPID");
   strcpy(vid[1],"CARRIERID");
   strcpy(vid[2],"CATEGORY");
   strcpy(vid[3],"NUMBER");
   strcpy(vid[4],_TOK_TAGID);
   strcpy(vid[5],_TOK_TAGDATA);
   vidArraySize=6;
   siz=strlen(_TOK_LOCNAME);
   pgnumber = 0;
   pglength = 30;
   
   do
   {
      pgnumber++;
      
      PRS_VFI_InitCommand("STATUS_QUERY", TRANSNET_NAME);
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CMDTYP,_SIZ_CMDTYPE, "LIST_DB");
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_AMHSEQUIPID,_SIZ_AMHSEQUIPID, stocker);
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_KEYATTRIBUTE,_SIZ_KEYATTRIBUTE, "ALL");
      if (TRC_ChkAlarm()) PRS_VFI_PutArray (_TOK_VARID,_SIZ_VID,vidArraySize,vid);
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_PAGENUMBER,_PRS_UINT, &pgnumber);
      if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_PAGELENGTH,_PRS_UINT, &pglength);
      
      message.msgType = _TLK_TYPVFEI;
      
      if (TLK_DMQ_ClientPut (hTransNetServer) != _RET_SUCCESS) 
      {
         TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: FAB3StockerDbList DMQPut Status = %d\t%s\n",
            TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_TRANSNET_INIT_FAIL));
      }
      
      if (TLK_DMQ_ClientGet (hTransNetServer) != _RET_SUCCESS) 
      {
         TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: FAB3StockerDbList DMQGet Status = %d\t%s\n",
            TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_TRANSNET_INIT_FAIL));
      }
      
      if (PRS_VFI_ChkStatus() != _RET_SUCCESS) 
      {
         TRC_Send(LOG_ALIAS, _TRC_LVL_WARN, "%s: FAB3StockerDbList Status = %d\t%s\n",
            TRC_GetTime(), _RET_TRANSNET_INIT_FAIL, _TRC_TRANSNET_INIT_FAIL);
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_TRANSNET_INIT_FAIL));
      }
      
      if (PRS_VFI_GetToken("CMD", _SIZ_MSGBUFF, cmd) != _RET_SUCCESS) 
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, "CMD"));
      
      if (strcmp(cmd, "STATUS_LIST") != 0) 
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_CMDWRONG, cmd, "STATUS_LIST"));
      if (PRS_VFI_GetToken("PAGELENGTH", _PRS_UINT, &tmppglength) != _RET_SUCCESS) 
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, "PAGELENGTH"));
      if (PRS_VFI_GetArray("AMHSEQUIPID",_MAX_CARRIER, _SIZ_AMHSEQUIPID,&count,tmpamhsequipid) != _RET_SUCCESS)
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, "AMHSEQUIPID"));
      if (PRS_VFI_GetArray("CARRIERID",_MAX_CARRIER, _SIZ_CARRIERID, &count, tmpcarrierid) != _RET_SUCCESS)
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, "CARRIERID"));
      if (PRS_VFI_GetArray(_TOK_CATEGORY,_MAX_CARRIER, _SIZ_CATEGORY,&count, tmpcategory) != _RET_SUCCESS)
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, _TOK_CATEGORY));
      if (PRS_VFI_GetArray(_TOK_NUMBER,_MAX_CARRIER, _SIZ_TMPNUMBER, &count, dummynumber) != _RET_SUCCESS)
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, _TOK_NUMBER));
      for (i=0;i<count;i++)
      {
         
         if (strcmp(tmpcategory[i],"SHELF")==0 && 
            strlen(dummynumber[i]) > 7)
         {
            sscanf(dummynumber[i],"%3s%2s%1s%3s%1s%3s",s4,s1,s4,s2,s4,s3);
            sprintf(tmpnumber[i],"%s%s%s",s1,s2,s3);
         }
         else
            sprintf(tmpnumber[i],"%s",dummynumber[i]);
         
      }
      if (PRS_VFI_GetArray(_TOK_TAGID,_MAX_CARRIER, _SIZ_LOCNAME, &tagidArraySize, tagid) != _RET_SUCCESS)
         return (TRC_SetAlarm("FAB3StockerDbList", _RET_FLDMISSING, _TOK_LOCNAME));
      /*locate which one is "PROMISLOC" in the list of TAG_IDs */
      for(TokLocnamePos=0;(TokLocnamePos<tagidArraySize);++TokLocnamePos)
         if (!strncmp(tagid[TokLocnamePos], _TOK_LOCNAME, siz)) 
            break;
         
         if ( TokLocnamePos == tagidArraySize)
         {
         /*
         Array of TAG_IDs does not contain a tag called "PROMISLOC" (_TOK_LOCNAME)
         therefore signal error and return
            */
            return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_LOCNAME));
         }
         if ( PRS_VFI_GetArray( _TOK_TAGDATA, _MAX_CARRIER * _NUM_TAGS, _SIZ_LOCNAME,
            &tagdataArraySize, tagdata) != _RET_SUCCESS)
            return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_LOCNAME));
         
         FAB3count=0;
         for( i=0; TokLocnamePos<tagdataArraySize; TokLocnamePos+=tagidArraySize, ++i)
         {
            // changes for blank promis location to be skipped
            if(strlen(tagdata[TokLocnamePos])==0)
               continue;
            if(!IsSMPLoc(tagdata[TokLocnamePos]))
            {
               strcpy( FAB3CarrierId[FAB3count], tmpcarrierid[i]);
               strcpy( FAB3equipment[FAB3count], tmpamhsequipid[i]);
               strcpy(tmpcategory[i],"");
               sprintf( FAB3LocationId[FAB3count], "%s%s", tmpcategory[i], tmpnumber[i]);
               ++FAB3count;
            }
         }
         /* Check if any lots were found before sending message */
         if(FAB3count != 0)
            retval=FAB3CreateSendF3Event( _TOK_FAB3ALLLOTLOC, FAB3count, FAB3CarrierId, FAB3LocationId, FAB3equipment);
         //           if(!retval)
         //                TRC_Send( LOG_ALIAS, _TRC_LVL_STATE, 
         //                          "%s: FAB3StockerDbList Status = %d\tAlarmCode[%d]AlarmMsg[%s]\n",
         //                       TRC_GetTime(), retval, TRC_GetAlarm(), TRC_ErrorMsg());
         
        }while (pglength == count);
        /* we still check count here as that is the
        * original number of values got from the message */
        return (retval);
}

/***** END Function **************************************************************************/

/*********************************************************************************************
Function              :       int chkPromisLoc( void )

  Parameters    : 
  void
  
  Returns               : int
    _RET_FAB3LOCATION
    _RET_SMPLOCATION
    error
    
  Functionality :       This function is called from function parsing carrier_entered
      carrier_removed and loc_changed
      
  Revision History:
  ~~~~~~~~~~~~~~~~
  Name  Date    Change done
  ----  ------- -------------------------------------------------------------------------------
  Ivan  000926  Started coding this function to add to existing PRISvr.
        
*********************************************************************************************/
          
int chkPromisLoc( void )
{
   char tagid[_NUM_TAGS][_SIZ_LOCNAME]={0}; 
   char tagdata[_NUM_TAGS][_SIZ_LOCNAME]={0}; 
   int tagidArraySize=0, i=0, siz=0;
   
   siz=strlen(_TOK_LOCNAME);
   if (PRS_VFI_GetArray(_TOK_TAGID, _NUM_TAGS, _SIZ_LOCNAME, &tagidArraySize, tagid) != _RET_SUCCESS)
      return (TRC_SetAlarm("DB List", _RET_FLDMISSING, _TOK_TAGID));
   
   for(i=0;i<tagidArraySize;++i)
      if (!strncmp(tagid[i], _TOK_LOCNAME, siz)) 
         break;
      
      if ( i == tagidArraySize)
      {
      /*
      If i == total elements, it means that array of TAG_IDs does not contain 
      a tag called "PROMISLOC" (_TOK_LOCNAME) therefore signal error and return
         */
         return (TRC_SetAlarm("chkPromisLoc()", _RET_FLDMISSING, _TOK_LOCNAME));
      }
      
      if (PRS_VFI_GetArray(_TOK_TAGDATA, _NUM_TAGS, _SIZ_LOCNAME, &tagidArraySize, tagdata) != _RET_SUCCESS)
         return (TRC_SetAlarm("chkPromisLoc()", _RET_FLDMISSING, _TOK_LOCNAME));
      
      if(strlen(tagdata[i])==0) //manage test lots' null locations Ivan 287364
         return -1;
      if(IsSMPLoc(tagdata[i]))  //PROMISLOC in tag data
         return _RET_SMPLOCATION;
      else
         return _RET_FAB3LOCATION;
}
/***** END Function **************************************************************************/

int FAB3MethodMove(char *LotId, char *EqptId, char *OutPort, char *UserId, int *pristatus, char *ErrorMsg)
{
   char cmd[_SIZ_GENFIELD]={0};
   int  ecd=0, statCode=0;
   
   TRC_ClrAlarm();
   
   PRS_VFI_InitCommand("MACH_CMD",TRANSNET_NAME);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CMDTYP,_SIZ_CMDTYPE, "MOVE");
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_CARRIERID,_SIZ_CARRIERID, LotId);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_DESTID,_SIZ_DESTID, EqptId);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_USERID, _SIZ_USERID, UserId);
   if (TRC_ChkAlarm()) PRS_VFI_PutToken (_TOK_OUTPUTPORT,_SIZ_OUTPUTPORT,OutPort);
   message.msgType = _TLK_TYPVFEI;

   if (TLK_DMQ_ClientPut (hGatewayServer) != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
   if (TLK_DMQ_ClientGet (hGatewayServer) != _RET_SUCCESS) 
   {
      *pristatus=4;
      strcpy(ErrorMsg,"Transnet timeout ");
      return (TRC_GetAlarm());
   }
   
   if ( PRS_VFI_ChkStatus() != _RET_SUCCESS) 
      return (TRC_GetAlarm());
   
   if (PRS_VFI_GetToken("CMD", _SIZ_GENFIELD, cmd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "CMD"));
   
   if (strncmp(cmd, "CMD_ACK", strlen("CMD_ACK")) != 0)
      return (TRC_SetAlarm("MOVE", _RET_CMDWRONG, cmd, "CMD_ACK"));
   
   if (PRS_VFI_GetToken("ECD", _PRS_UINT, &ecd) != _RET_SUCCESS) 
      return (TRC_SetAlarm("MOVE", _RET_FLDMISSING, "ECD"));
   
   if (ecd != 0) 
      *pristatus=ecd;
   return (TRC_ChkAlarm());
}
/*.............................................................*/
char *rTrim(char *tStr)
{
   int iCnt;
   iCnt=strlen(tStr)-1;
   for(;tStr[iCnt]==32;--tStr)
         tStr[iCnt]=0;
  return tStr;
}
/*.............................................................*/
int Fab3Stocker(char *ChkId)
{
   int retval=-1;
   struct iList *curr=NULL;
   
   /***  Loop till end of list or retval = 0 (false)
   ***/  
   for(curr=fab3StockerStart;(curr) && retval; curr=curr->next)
      retval=strcmp(curr->iName,ChkId);
   return (retval==0);
}
