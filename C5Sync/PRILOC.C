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

char *rTrim(char *tStr)
{
   int iCnt;
   iCnt=strlen(tStr)-1;
   for(;tStr[iCnt]==32;--tStr)
         tStr[iCnt]=0;
  return tStr;
}
/*.............................................................*/

