/******************************************************************************/
/*                                                                            */
/*   talkDMQ.c:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains the functions required to communicate with            */
/*   applications on the DMQ.                                                 */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* The following are the standard include statements.                         */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef VMS
#define WINNTAPI 
#include <varargs.h>
#else
#include <stdarg.h>
#define WINNTAPI FAR PASCAL
#endif


/* End of standard includes.                                                  */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "ordarray.h"

#include "talkutl.h"
#include "trcutil.h"
#include "parsutl.h"

/* End of include statements for local files.                                 */
/******************************************************************************/

/*****VERSION*********************************************************/

static char talkDMQcid[] = "$Id: talkDMQ.c Ver 01.0 Rel 01.0 $";

static int32 DmqTimerId=2000;

int ExtRecdTimerId;

/*********************************************************************/

/******************************************************************************/
/* The following are global variable definitions.                             */

OAR_array *dmqHandle = NULL;

OAR_array *tgtHandle = NULL;

int32 dmqSrvTimeout = 0;     /* set as global in case override is required */

/* End of global variable definitions.                                        */
/******************************************************************************/


/*******************************************************************************

   TLK_DMQ_SetQueue:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function should be called to setup a DMQ queue.  The queue setup by
   this function may be used to receive commands as well as replies.

      Arguments:
      ~~~~~~~~~~
         - handle (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the queue.

         - qName (type is *char - Read only):
              The referenced string is the queue name to be used.  The value
              of qName can either be the queue name or number, depending on
              the value of attMode.  If the attach mode is temporary, this
              field is used for logging purposes only.

         - attMode (type is long int - Read only):
              The value of attMode represent the attach mode to be used by
              DECmessageQ.  This field can be one of PSYM_ATTACH_BY_NAME,
              PSYM_ATTACH_BY_NUMBER or PSYM_ATTACH_TEMPORARY.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_SetQueue (int  handle,
                      char *qName,
                      long int attMode,
                      char *logFile,
                      int  verbose) {

  TLK_dmqRec *dmqRec;
  long int  pamsStatus;
/*  long int  nameLen; */
  int32  nameLen;
  

  /*..........................................................................*/
  /* Validate the inputs for length...                                        */
  
  if (strlen (qName) >= _SIZ_DMQNAME)
    return (TRC_SetAlarm ("TLK_DMQ_SetQueue", _RET_FLDTOOLONG,
                          "queue name", _SIZ_DMQNAME));
  
  /*..........................................................................*/
  /* Check if array of queues is created (create if required)...              */

  if (dmqHandle == NULL)
    if ((dmqHandle = OAR_Open (1, 1,
                              (LP_locate) TLK_DMQ_LocQueue, (LP_compare) TLK_DMQ_CmpQueue)) == NULL)
      return (TRC_SetAlarm ("TLK_DMQ_SetQueue", _RET_NOMEMORY));

  /*..........................................................................*/
  /* If handle exists, signal an error...                                     */

  if ((dmqRec = OAR_GetElement (dmqHandle, &handle)) != NULL)
    return (TRC_SetAlarm ("TLK_DMQ_SetQueue", _RET_RECEXISTS, "DMQ handle"));

  /*..........................................................................*/
  /* Create the handle record...                                              */
  
  if ((dmqRec = (TLK_dmqRec *) malloc (sizeof (TLK_dmqRec))) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
  memset (dmqRec, 0, sizeof (TLK_dmqRec));

  /*..........................................................................*/
  /* Set new values for the handle...                                         */

  dmqRec->handle = handle;
  strcpy (dmqRec->name, qName);
  dmqRec->attachMode = attMode;
  strcpy(dmqRec->logFile,logFile);
  dmqRec->verbose = verbose;

  if (OAR_AddElement (dmqHandle, dmqRec) == _OAR_INVALIDPOS) {
    free (dmqRec);
    return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
  }

  /*..........................................................................*/
  /* Attempt to attach to the queue...                                        */
  
  nameLen = strlen (dmqRec->name);
  dmqRec->attachType = PSYM_ATTACH_PQ;
  dmqRec->filter.au.group = (unsigned short) PSEL_PQ;
  dmqRec->filter.au.queue = 0;
  if (dmqRec->attachMode != PSYM_ATTACH_TEMPORARY)
    pamsStatus = pams_attach_q ( &dmqRec->attachMode, &dmqRec->queue,
                                &dmqRec->attachType, dmqRec->name,
                                &nameLen, 0, 0, 0, 0, 0);
  else
    pamsStatus = pams_attach_q (&dmqRec->attachMode, &dmqRec->queue,
                                &dmqRec->attachType, 0, 0, 0, 0, 0, 0, 0);
  
  if (pamsStatus == PAMS__BADDECLARE) {
    dmqRec->attachType = PSYM_ATTACH_SQ;
    if (dmqRec->attachMode != PSYM_ATTACH_TEMPORARY)
      pamsStatus = pams_attach_q (&dmqRec->attachMode, &dmqRec->queue,
                                  &dmqRec->attachType, dmqRec->name,
                                  &nameLen, 0, 0, 0, 0, 0);
    else 
      pamsStatus = pams_attach_q (&dmqRec->attachMode, &dmqRec->queue,
                                  &dmqRec->attachType, 0, 0, 0, 0, 0, 0, 0);

    dmqRec->filter.au.group = (unsigned short) PSEL_AQ;
    dmqRec->filter.au.queue = dmqRec->queue.au.queue;
  }

  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  if (pamsStatus == PAMS__SUCCESS)
    TRC_Send (logFile, verbose, _DMQ_TRC_CONNECT, TRC_GetTime (),
              dmqRec->name, dmqRec->queue.au.group, dmqRec->queue.au.queue);

  /**************************************************************************/
  /* Added on 22-07-98 for flushing the queue                               */
  TLK_DMQ_Flush (dmqRec);

  return (TLK_DMQ_SetError ("PAMS_ATTACH_Q", pamsStatus));
}

/********************** End of function TLK_DMQ_SetQueue **********************/


/*******************************************************************************

   TLK_DMQ_SetTarget:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function should be called to setup a channel for sending messages
   to a specified host.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the target.

         - hostName (type is *char - Read only):
              The referenced string is the mailbox name to use.

         - timeout (type is int - Read only):
              The value represents the number of seconds to wait for replies
              from the specified target.

         - handle (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the DMQ structure for replies.  If no replies are required,
              use _TLK_DMQ_NOQUEUE.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_SetTarget (int  target,
                       char *hostName,
                           int  rplyHandle,
                       int  timeout,
                       char *logFile,
                       int  verbose) {

  TLK_dmqTgt *dmqTgt;

  long int pamsStatus;
  int32 nameLen;
  long int waitMode;
  long int searchListLen = 4;
  long int searchList[4] = {PSEL_TBL_PROC, PSEL_TBL_GRP, 
                            PSEL_TBL_DNS_CACHE, PSEL_TBL_DNS_LOW};

  /*..........................................................................*/
  /* Validate the inputs for length...                                        */
  
  if (strlen (hostName) >= _SIZ_DMQNAME)
    return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_FLDTOOLONG,
                          "queue name", _SIZ_DMQNAME));
  
  /*..........................................................................*/
  /* Check if array of targets is created (create if required)...             */
  
  if (tgtHandle == NULL)
    if ((tgtHandle = OAR_Open (1, 1, (LP_locate) TLK_DMQ_LocQTgt, (LP_compare) TLK_DMQ_CmpQTgt)) == NULL)
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));

  /*..........................................................................*/
  /* Get existing target handle, or create a new one...                       */

  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL) {
    if ((dmqTgt = (TLK_dmqTgt *) malloc (sizeof (TLK_dmqTgt))) == NULL)
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
    memset (dmqTgt, 0, sizeof (TLK_dmqTgt));
    dmqTgt->target = target;
    if (OAR_AddElement (tgtHandle, dmqTgt) == _OAR_INVALIDPOS) {
      free (dmqTgt);
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
    }
  }

  /*..........................................................................*/
  /* Set new values for the handle...                                         */

  strcpy (dmqTgt->name, hostName);
  dmqTgt->timeout= timeout * 10;
  dmqTgt->rplyHandle = rplyHandle;
  strcpy (dmqTgt->logFile, logFile);
  dmqTgt->verbose = verbose;
  
  
  /*..........................................................................*/
  /* Get the address of of DMQ host queue...                                  */
  
  nameLen = strlen (hostName);
  waitMode = PSYM_WF_RESP;
#ifdef VMS
  pamsStatus = pams_locate_q (hostName, &nameLen, (q_address *)&dmqTgt->queue.all,
                              0, 0, 0, 0, 0, 0);
#else
  pamsStatus = pams_locate_q (hostName, &nameLen, (q_address *)&dmqTgt->queue.all,
                              &waitMode, NULL, NULL, &searchList[1],
                              &searchListLen, NULL);
#endif

  if (pamsStatus != PAMS__SUCCESS) {
    if ((dmqTgt = OAR_DelElement (tgtHandle, &target)) != NULL)
      free (dmqTgt);
  } else
    TRC_Send (logFile, verbose, _DMQ_TRC_CONTARG, TRC_GetTime (),
              dmqTgt->name, dmqTgt->queue.au.group, dmqTgt->queue.au.queue);

  return (TLK_DMQ_SetError ("PAMS_LOCATE_Q", pamsStatus));
}

/********************* End of function TLK_DMQ_SetTarget **********************/
/*******************************************************************************

   TLK_DMQ_GetQDetail:                                 Date:  May 1998
   ~~~~~~~~~~~~~~~~~~                               Author:  Nawal Parwal

   This function should be called to locate the DMQ permanent Queue's Groupid 
   and QueueID
   
      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the target.

         - hostName (type is *char - Read only):
              The referenced string is the mailbox name to use.

         - timeout (type is int - Read only):
              The value represents the number of seconds to wait for replies
              from the specified target.

         - handle (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the DMQ structure for replies.  If no replies are required,
              use _TLK_DMQ_NOQUEUE.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_GetQDetail(int target,
                            int  *GroupId,
                        int  *QueueId,char *logFile,int verbose) {

  TLK_dmqRec *dmqRec;

  /*..........................................................................*/
  /* Get the address of of DMQ host queue...                                  */
  if ((dmqRec = OAR_GetElement (dmqHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_QetQDetail", _RET_RECNOTFND, "DMQ handle"));
          TRC_Send (logFile, verbose,"%s - Found Group=%d and Queue=%d\n", TRC_GetTime (),
              dmqRec->queue.au.group, dmqRec->queue.au.queue);
                *GroupId=(int)dmqRec->queue.au.group;
                *QueueId=(int)dmqRec->queue.au.queue;
  return (_RET_SUCCESS);
}

  /*..........................................................................*/
  /*..........................................................................*/

int TLK_DMQ_GetQ2Detail(int target,
                        q_address *que_addr,
                        char *logFile,int verbose) {

  TLK_dmqRec *dmqRec;

  /*..........................................................................*/
  /* Get the address of of DMQ host queue...                                  */
  if ((dmqRec = OAR_GetElement (dmqHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_QetQ2Detail", _RET_RECNOTFND, "DMQ handle"));
          TRC_Send (logFile, verbose,"%s - Found Group=%d and Queue=%d\n", TRC_GetTime (),
              dmqRec->queue.au.group, dmqRec->queue.au.queue);
                que_addr->au.group=(short)dmqRec->queue.au.group;
                que_addr->au.queue=(short)dmqRec->queue.au.queue;
  return (_RET_SUCCESS);
}

  /*..........................................................................*/
  /*..........................................................................*/

int TLK_DMQ_GetQ3Detail(int target,
                        q_address *que_addr,
                        char *logFile,int verbose) {

  TLK_dmqTgt *dmqTgt;

  /*..........................................................................*/
  /* Get the address of of DMQ host queue...                                  */
  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_QetQ2Detail", _RET_RECNOTFND, "DMQ handle"));
          TRC_Send (logFile, verbose,"%s - Found Group=%d and Queue=%d\n", TRC_GetTime (),
              dmqTgt->queue.au.group, dmqTgt->queue.au.queue);
                que_addr->au.group=(short)dmqTgt->queue.au.group;
                que_addr->au.queue=(short)dmqTgt->queue.au.queue;
  return (_RET_SUCCESS);
}

/********************* End of function TLK_DMQ_GetQDetail **********************/
/*******************************************************************************

   TLK_DMQ_SetTargetQNumber:                        Date:  May 1998
   ~~~~~~~~~~~~~~~~~~                               Author:  Nawal Parwal

   This function should be called to setup a channel for sending messages
   to a specified host.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the target.

         - Group Number (type is int - Read only):
              The Value is the Group Number name to use.

                 - Queue Number (type is int - Read only):
              The Value is the Queue Number name to use.

         - timeout (type is int - Read only):
              The value represents the number of seconds to wait for replies
              from the specified target.

         - handle (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the DMQ structure for replies.  If no replies are required,
              use _TLK_DMQ_NOQUEUE.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_SetTargetQNumber (int  target,
                       int group,
                           int queue,
                           int  rplyHandle,
                       int  timeout,
                       char *logFile,
                       int  verbose) {

  TLK_dmqTgt *dmqTgt;

  
  long int searchListLen = 4;
  long int searchList[4] = {PSEL_TBL_PROC, PSEL_TBL_GRP, 
                            PSEL_TBL_DNS_CACHE, PSEL_TBL_DNS_LOW};

  
  
  /*..........................................................................*/
  /* Check if array of targets is created (create if required)...             */
  
  if (tgtHandle == NULL)
    if ((tgtHandle = OAR_Open (1, 1, (LP_locate) TLK_DMQ_LocQTgt, (LP_compare) TLK_DMQ_CmpQTgt)) == NULL)
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));

  /*..........................................................................*/
  /* Get existing target handle, or create a new one...                       */

  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL) {
    if ((dmqTgt = (TLK_dmqTgt *) malloc (sizeof (TLK_dmqTgt))) == NULL)
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
    memset (dmqTgt, 0, sizeof (TLK_dmqTgt));
    dmqTgt->target = target;
    if (OAR_AddElement (tgtHandle, dmqTgt) == _OAR_INVALIDPOS) {
      free (dmqTgt);
      return (TRC_SetAlarm ("TLK_DMQ_SetTarget", _RET_NOMEMORY));
    }
  }

  /*..........................................................................*/
  /* Set new values for the handle...                                         */

  strcpy (dmqTgt->name, "");
  sprintf(dmqTgt->name,"%d:%d",group,queue);
  dmqTgt->timeout= timeout * 10;
  dmqTgt->rplyHandle = rplyHandle;
  strcpy (dmqTgt->logFile, logFile);
  dmqTgt->verbose = verbose;
  
  dmqTgt->queue.au.group=group;
  dmqTgt->queue.au.queue=queue;

  return (_RET_SUCCESS);
}

/********************* End of function TLK_DMQ_SetTargetQnumber **********************/


/*******************************************************************************

   TLK_DMQ_SetTimeout:                                Date:  July 1996
   ~~~~~~~~~~~~~~~~~~~                              Author:  Sebastien Spicer

   This function will set a new value for the timeout on a target.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the target.

         - timeout (type is int - Read only):
              The value represents the number of seconds to wait for replies
              from the specified target.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_SetTimeout (int target,
                        int timeout) {

  TLK_dmqTgt *dmqTgt;

  /*..........................................................................*/
  /* Check if array of targets is created...                                  */

  if (tgtHandle == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_SetTimeout", _RET_RECNOTFND, "target"));

  /*..........................................................................*/
  /* Get existing target handle...                                            */

  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_SetTimeout", _RET_RECNOTFND, "target"));

  /*..........................................................................*/
  /* Set new values for the handle...                                         */

  dmqTgt->timeout = timeout * 10;
  return (_RET_SUCCESS);
}

/********************* End of function TLK_DMQ_SetTimeout *********************/


/*******************************************************************************

   TLK_DMQ_ClrQueue:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function should be called to release a given DMQ handle and its
   associated queue.

      Arguments:
      ~~~~~~~~~~
         - handle (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the queue.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_ClrQueue (int  handle){

  TLK_dmqRec *dmqRec;
  long int  pamsStatus;
  int32 detach_opt_list[1];
  int32 detach_opt_len = 0;
  int32 msgs_flushed;

  /*..........................................................................*/
  /* Retrieve the DMQ structure associated with the handle...                 */

  if ((dmqRec = OAR_GetElement (dmqHandle, &handle)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ClrQueue", _RET_RECNOTFND, "DMQ handle"));

  /*..........................................................................*/
  /* Make sure that it is the last queue if primary...                        */

  if ((dmqRec->attachType == PSYM_ATTACH_PQ) && (OAR_Size (dmqHandle) > 1))
    return (TRC_SetAlarm ("TLK_DMQ_ClrQueue", _RET_DMQERROR,
                          "Primary queue must be last to be released"));
  
  /*..........................................................................*/
  /* Detach from the queue...                                                 */
  
  TRC_Send (dmqRec->logFile, dmqRec->verbose, _DMQ_TRC_RELEASE, TRC_GetTime (),
            dmqRec->name, dmqRec->queue.au.group, dmqRec->queue.au.queue);
  pamsStatus = pams_detach_q (&dmqRec->queue, detach_opt_list, &detach_opt_len,
                              &msgs_flushed);
  if ((dmqRec = OAR_DelElement (dmqHandle, &handle)) != NULL)
    free (dmqRec);

  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (TLK_DMQ_SetError ("PAMS_DETACH_Q", pamsStatus));
}

/********************** End of function TLK_DMQ_ClrQueue **********************/


/*******************************************************************************

   TLK_DMQ_ClrTarget:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function should be called to release a given target handle.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced code is the handle to be used when referencing
              the target.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_ClrTarget (int  target) {

  TLK_dmqTgt *dmqTgt;

  /*..........................................................................*/
  /* Retrieve the DMQ target associated with the handle...                    */

  if ((dmqTgt = OAR_DelElement (tgtHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ClrTarget", _RET_RECNOTFND, "DMQ target"));

  /*..........................................................................*/
  /* Detach from the queue...                                                 */
  
  TRC_Send (dmqTgt->logFile, dmqTgt->verbose, _DMQ_TRC_RELTARG, TRC_GetTime (),
            dmqTgt->name, dmqTgt->queue.au.group, dmqTgt->queue.au.queue);
  free (dmqTgt);

  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (_RET_SUCCESS);
}

/********************** End of function TLK_DMQ_ClrTarget *********************/


/*******************************************************************************

   TLK_DMQ_Shutdown:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function shuts down DMQ for the application.

      Arguments:
      ~~~~~~~~~~
         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void TLK_DMQ_Shutdown (void) {

  long int pamsStatus;
  

  /*..........................................................................*/
  /* Shutdown DECmessageQ...                                                  */

  pamsStatus = PAMS__SUCCESS;
  if ((dmqHandle != NULL) && (OAR_Size (dmqHandle) > 0)) {
    pamsStatus = pams_exit ();
    OAR_Close (dmqHandle);
    OAR_Close (tgtHandle);
    dmqHandle = NULL;
    tgtHandle = NULL;
  }

  return;
}

/*********************** End of function TLK_DMQ_Shutdown *********************/


/*******************************************************************************

   TLK_DMQ_ClientPut:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function should be called so that a client may submit a query to a
   host.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced string is the handle for referencing the target.

         - logFile (type is *char - Read only):
              The referenced string contains the alias to be used for logging.

         - verbose (type is int - Read only):
              The integer value represents the verbose level to be used.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_ClientPut (int target){

  TLK_dmqTgt *dmqTgt;
  TLK_dmqRec *dmqRec, dmqTmp;

  long int pamsStatus;
  int32 timeout;
  short int buffLen;
  int index;

  char delivery = PDEL_MODE_NN_MEM;
  char uma = PDEL_UMA_DISC;
  char priority = 0;
 
  
  /*..........................................................................*/
  /* Get target and queue handles...                                          */

  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ClientPut", _RET_RECNOTFND, "DMQ target"));
  
  if ((dmqRec = OAR_GetElement (dmqHandle, &dmqTgt->rplyHandle)) == NULL) 
  {
    memset (&dmqTmp, 0, sizeof (TLK_dmqRec));
    dmqTmp.dmqclass = MSG_CLAS_PAMS;
  }
  else
  {
        dmqRec->dmqtype=0;
        dmqRec->dmqclass = 0;
  }
  strcpy (message.host, dmqTgt->name);
 
  /*..........................................................................*/
  /* Log the outgoing message...                                              */

  switch (message.msgType) {
  
  case _TLK_TYPVFEI:
    PRS_VFI_LogOutMsg (dmqTgt->logFile, dmqTgt->verbose);
    break;

  default:
    TLK_LogOutMsg (dmqTgt->logFile, dmqTgt->verbose);
    break;
  }
 
  /*..........................................................................*/
  /* Attempt to write message to DMQ...                                       */
  
  index = _TLK_DMQ_RETRY;
  timeout =dmqTgt->timeout;
  buffLen = message.msgLen;
  
  do {
    if (dmqRec != NULL) {
      TLK_DMQ_Flush (dmqRec);
      pamsStatus = pams_put_msg (message.buffer, &priority, &dmqTgt->queue,
                                 &dmqRec->dmqclass, &dmqRec->dmqtype, &delivery,
                                 &buffLen, (int32 *) &timeout, &dmqRec->psb, &uma,
                                 &dmqRec->queue, 0, 0, 0);
    } else
      pamsStatus = pams_put_msg (message.buffer, &priority, &dmqTgt->queue,
                                 &dmqTmp.dmqclass, &dmqTmp.dmqtype, &delivery,
                                 &buffLen, &timeout, &dmqTmp.psb, &uma,
                                 &dmqTmp.queue, 0, 0, 0);
    if (pamsStatus != PAMS__SUCCESS)
      sleep (1);
  } while ((pamsStatus != PAMS__SUCCESS) && (--index != 0));
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (TLK_DMQ_SetError ("PAMS_PUT_MSG", pamsStatus));
}

/********************** End of function TLK_DMQ_ClientPut *********************/


/*******************************************************************************

   TLK_DMQ_ClientGet:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function should be called so that a client may receive a reply from
   a host.

      Arguments:
      ~~~~~~~~~~
         - target (type is int - Read only):
              The referenced string is the handle referencing the target.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_ClientGet (int target) {


  TLK_dmqTgt *dmqTgt;
  TLK_dmqRec *dmqRec;
  int seqFlag;
  q_address localqueue;                   /* local queue number                   */

  long int   pamsStatus;
  int32   timeout;
  short int  buffLen;
  short      bufferSize = _SIZ_MSGBUFF;
  char       priority = 0;
        
  /*..........................................................................*/
  /* Get target and queue handles...                                          */

  if ((dmqTgt = OAR_GetElement (tgtHandle, &target)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ClientPut", _RET_RECNOTFND, "DMQ target"));
  dmqRec = OAR_GetElement (dmqHandle, &dmqTgt->rplyHandle);
  timeout = dmqTgt->timeout;
        
  /*..........................................................................*/
  /* Exit if no reply is required...                                          */

  if ((dmqRec == NULL) || (dmqTgt->timeout <= 0)) {
    message.buffer[0] = 0;
    message.msgLen = message.msgPtr = 0;
    return (_RET_SUCCESS);
  }

  /*..........................................................................*/
  /* Loop until right message is received (or error)...                       */

  do {


    /*........................................................................*/
    /* Attempt to retrieve a message from DMQ...                              */
    
   


        pamsStatus = pams_get_msgw (message.buffer, &priority,&localqueue,
                                &dmqRec->dmqclass, &dmqRec->dmqtype, &bufferSize,
                                &buffLen, &timeout, (int32 *) &dmqRec->filter,
                                &dmqRec->psb, 0, 0, 0, 0, 0);
 

        /*........................................................................*/
    /* Setup the message fields based on retrieved data...                    */
    
    if (pamsStatus == PAMS__SUCCESS) {
      message.msgLen = buffLen;
      message.buffer[message.msgLen] = 0;
      message.msgPtr = 0;
    } else {
      strcpy (message.host, "client");
      message.buffer[0] = 0;
      message.msgLen = message.msgPtr = 0;
      return (TLK_DMQ_SetError ("PAMS_GET_MSGW", pamsStatus));
    }
        ExtRecdTimerId = 0;
        if ( dmqRec->dmqtype == MSG_TYPE_TIMER_EXPIRED )
        memcpy ( &ExtRecdTimerId ,message.buffer,sizeof(int32) );
    /*........................................................................*/
    /* Log the incomming message...                                           */
    
    seqFlag = _RET_SUCCESS;
    switch (message.msgType) {
    case _TLK_TYPPTP:
      PRS_PTP_LogInpMsg (dmqTgt->logFile, dmqTgt->verbose);
      break;
    case _TLK_TYPVFEI:
      PRS_VFI_LogInpMsg (dmqTgt->logFile, dmqTgt->verbose);
      seqFlag = PRS_VFI_ChkMsgSeq ();
      break;
   
        default:
                
           TLK_LogInpMsg (dmqRec->logFile, dmqRec->verbose);
      break;
          
    
    }
    message.msgPtr = 0;

    if (seqFlag == _RET_PARSESEQ)
      TRC_ClrAlarm ();

  } while (seqFlag != 0);
    
  return (seqFlag);
}

/********************** End of function TLK_DMQ_ClientGet *********************/


/*******************************************************************************

   TLK_DMQ_ServerGet:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function performs a read on a server queue.  The read will be a
   blocking read.

      Arguments:
      ~~~~~~~~~~
         - handle (type is int - Read only):
              The referenced code is the handle for the command queue.

         - msgType (type is int - Read only):
              The value indicates what kind of message is expected from the
              sending application.

         - logFile (type is *char - Read only):
              The referenced string is the alias for the log file when
              logging messages.

         - verbose (type is int - Read only):
              The value represents the verbose level for logging outbound
              messages.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_ServerGet (int  handle,
                       int  msgType, char *msgstr){
                       

  TLK_dmqRec *dmqRec;
  long int   pamsStatus;
  short int  buffLen;
  short      bufferSize = _SIZ_MSGBUFF;
  char       priority = 0;
  

  /*..........................................................................*/
  /* Retrieve the DMQ structure associated with the handle...                 */

  if ((dmqRec = OAR_GetElement (dmqHandle, &handle)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ServerGet", _RET_RECNOTFND, "DMQ handle"));

  /*..........................................................................*/
  /* Attempt to retrieve a message from DMQ...                                */
  
  pamsStatus = pams_get_msgw (message.buffer, &priority, &dmqRec->rplyQueue,
                              &dmqRec->dmqclass, &dmqRec->dmqtype, &bufferSize,
                              &buffLen, &dmqSrvTimeout, (int32 *) &dmqRec->filter,
                              &dmqRec->psb, 0, 0, 0, 0, 0);

  /*..........................................................................*/
  /* Setup the message fields based on retrieved data...                      */
  
  message.msgType = msgType;

  

  if (pamsStatus == PAMS__SUCCESS) {
    sprintf (message.host, "%d.%d", dmqRec->rplyQueue.au.group,
             dmqRec->rplyQueue.au.queue);
    message.msgLen = buffLen;
    message.buffer[message.msgLen] = 0;
        strcpy(msgstr,message.buffer);
    message.msgPtr = 0;
  } else {
    strcpy (message.host, "client");
    message.buffer[0] = 0;
        strcpy(msgstr,"");
    message.msgLen = message.msgPtr = 0;
  }
        ExtRecdTimerId = 0;
        if ( dmqRec->dmqtype == MSG_TYPE_TIMER_EXPIRED )
                memcpy ( &ExtRecdTimerId ,message.buffer,sizeof(int32) );
        
  /*..........................................................................*/
  /* Log the incomming message if successful...                               */
  
  if (pamsStatus == PAMS__SUCCESS) {
    switch (message.msgType) {
   
   case _TLK_TYPVFEI:
           if ( dmqRec->dmqtype != MSG_TYPE_TIMER_EXPIRED) 
          PRS_VFI_ChkInpMsg() ;
      PRS_VFI_LogInpMsg (dmqRec->logFile, dmqRec->verbose);
      break;
          default:
                
           TLK_LogInpMsg (dmqRec->logFile, dmqRec->verbose);
      break;
          
    }
    message.msgPtr = 0;
  }
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (TLK_DMQ_SetError ("PAMS_GET_MSGW", pamsStatus));
}
  
int  TLK_DMQ_ServerGeta (       int  handle,
                                int  msgType,
                                void (*DmqDispatch)(), TLK_dmqRec **dmqRec1 )
{
                       

  TLK_dmqRec *dmqRec;

  char       priority ;

  long int   pamsStatus;

  short      bufferSize ;

  bufferSize = _SIZ_MSGBUFF;

  priority = 0;

  /*..........................................................................*/
  /* Retrieve the DMQ structure associated with the handle...                 */

  if ((dmqRec = OAR_GetElement (dmqHandle, &handle)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ServerGet", _RET_RECNOTFND, "DMQ handle"));

  *dmqRec1 = dmqRec;
  /*..........................................................................*/
  /* Attempt to retrieve a message from DMQ...                                */
  pamsStatus = pams_get_msga (message.buffer, &priority, &dmqRec->rplyQueue,
                              &dmqRec->dmqclass, &dmqRec->dmqtype, &bufferSize,
                              (short *) &message.msgLen,  (int32 *) &dmqRec->filter,
                              &dmqRec->psb, 0, 0, 0, 0, DmqDispatch,0,
                              0, 0);

}
/********************* End of function TLK_DMQ_ServerGet **********************/

/*******************************************************************************

   TLK_DMQ_ServerGetNoWait:                                 Date:  July 1997
   ~~~~~~~~~~~~~~~~~~                               Author:  Jayaram

   This function performs a read on a server queue.  The read will be a
   non blocking read.

      Arguments:
      ~~~~~~~~~~
         - handle (type is int - Read only):
              The referenced code is the handle for the command queue.

         - msgType (type is int - Read only):
              The value indicates what kind of message is expected from the
              sending application.


      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_ServerGetNoWait (int  handle,
                       int  msgType) {
                    

  TLK_dmqRec *dmqRec;
  long int   pamsStatus;
  short int  buffLen;
  short      bufferSize = _SIZ_MSGBUFF;
  char       priority = 0;

  /*..........................................................................*/
  /* Retrieve the DMQ structure associated with the handle...                 */

  if ((dmqRec = (TLK_dmqRec *) OAR_GetElement (dmqHandle, &handle)) == NULL)
    return (TRC_SetAlarm ("TLK_ServerGet", _RET_RECNOTFND, "DMQ handle"));

  /*..........................................................................*/
  /* Attempt to retrieve a message from DMQ...                                */
  
  pamsStatus = pams_get_msg (message.buffer, &priority, &dmqRec->rplyQueue,
                              &dmqRec->dmqclass, &dmqRec->dmqtype, &bufferSize,
                              &buffLen, (int32 *) &dmqRec->filter,
                              &dmqRec->psb, 0, 0, 0, 0, 0);

  /*..........................................................................*/
  /* Setup the message fields based on retrieved data...                      */
  
  message.msgType = msgType;
  
  if (pamsStatus == PAMS__SUCCESS) {
    sprintf (message.host, "%d.%d", dmqRec->rplyQueue.au.group,
             dmqRec->rplyQueue.au.queue);
    message.msgLen = buffLen;
    message.buffer[message.msgLen] = 0;
    message.msgPtr = 0;
  } else {
    strcpy (message.host, "client");
    message.buffer[0] = 0;
    message.msgLen = message.msgPtr = 0;
  }

  ExtRecdTimerId = 0;
  if ( dmqRec->dmqtype == MSG_TYPE_TIMER_EXPIRED )
        memcpy ( &ExtRecdTimerId ,message.buffer,sizeof(int32) );

  /*..........................................................................*/
  /* Log the incomming message if successful...                               */
  
  if (pamsStatus == PAMS__SUCCESS) {
    switch (message.msgType) {
        
        
    
        case _TLK_TYPVFEI:
                        PRS_VFI_ChkInpMsg() ;
                  PRS_VFI_LogInpMsg (dmqRec->logFile, dmqRec->verbose);
      break;
        default:
                TLK_LogInpMsg (dmqRec->logFile, dmqRec->verbose);
      break;
    
    }
    message.msgPtr = 0;
  }
  


  /*..........................................................................*/
  /* Report on the success status...                                          */
   if (pamsStatus == PAMS__NOMOREMSG) {
      return _RET_DMQNOMOREMSG;
  }

  return (TLK_DMQ_SetError ("PAMS_GET_MSG", pamsStatus));
}

/********************* End of function TLK_GetCommand **********************/

/*******************************************************************************

   TLK_DMQ_ServerPut:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function performs final reply for a server transaction.  It returns
   the contents of the message buffer to the specified client.

      Arguments:
      ~~~~~~~~~~
         - handle (type is int - Read only):
              The referenced code is the handle for the command queue that
              was used to receive the server command.

         - logFile (type is *char - Read only):
              The referenced string is the alias for the log file when
              logging messages.

         - verbose (type is int - Read only):
              The value represents the verbose level for logging outbound
              messages.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_ServerPut (int  handle) {
                      

  TLK_dmqRec *dmqRec;
  long int pamsStatus;
  short int buffLen;
  int status;
  int index;

  long int timeout = 0;
  char delivery = PDEL_MODE_WF_MEM;
  char uma = PDEL_UMA_DISC;
  char priority = 0;

  /*..........................................................................*/
  /* Retrieve the DMQ structure associated with the handle...                 */

  if ((dmqRec = OAR_GetElement (dmqHandle, &handle)) == NULL)
    return (TRC_SetAlarm ("TLK_DMQ_ServerPut", _RET_RECNOTFND, "DMQ handle"));

  /*..........................................................................*/
  /* If no alarm was set, confirm possible recoverable messages...            */
  
  if (TRC_ChkAlarm())
    if ((status = TLK_DMQ_Confirm (dmqRec)) != _RET_SUCCESS)
      return (status);

  /*..........................................................................*/
  /* If no reply was requested, do not return data...                         */
  
  if (dmqRec->dmqclass == _TLK_DMQ_NORPLY)
    return (_RET_SUCCESS);

  /*..........................................................................*/
  /* Log the outgoing message...                                              */

  switch (message.msgType) {
  
  default:
    TLK_LogOutMsg (dmqRec->logFile, dmqRec->verbose);
    break;
  }

  /*..........................................................................*/
  /* Attempt to write message to DMQ...                                       */
  
  index = _TLK_DMQ_RETRY;
  buffLen = message.msgLen;
  do {
    pamsStatus = pams_put_msg (message.buffer, &priority, &dmqRec->rplyQueue,
                               &dmqRec->dmqclass, &dmqRec->dmqtype, &delivery,
                               &buffLen, (int32 *)&timeout, &dmqRec->psb, &uma,
                               &dmqRec->queue, 0, 0, 0);
    if (pamsStatus != PAMS__SUCCESS)
      sleep (1);
  } while ((pamsStatus != PAMS__SUCCESS) && (--index != 0));
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (TLK_DMQ_SetError ("PAMS_PUT_MSG", pamsStatus));
}

/********************* End of function TLK_DMQ_ServerPut **********************/


/*******************************************************************************

   TLK_DMQ_Flush:                                     Date:  July 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will flush a queue of pending messages.

      Arguments:
      ~~~~~~~~~~
         - dmqRec (type is *TLK_dmqRec - Read/Write):
              The referenced structure contains fields for communicating with
              the associated queue.  Some fields (for internal use) may be
              modified.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TLK_DMQ_Flush (TLK_dmqRec *dmqRec) {

  TLK_dmqRec dmqTmp;
  long int   pamsStatus;
  short int  buffLen;
  short      bufferSize = _SIZ_MSGBUFF;
  char       buffer[_SIZ_MSGBUFF];
  char       priority = 0;

  /*..........................................................................*/
  /* Exit if no queue is defined...                                           */

  if (dmqRec == NULL)
    return;
  memset (&dmqTmp, 0, sizeof (TLK_dmqRec));

  /*..........................................................................*/
  /* Loop until all messages are gone (or error)...                           */

  do {
    pamsStatus = pams_get_msg (buffer, &priority, &dmqTmp.rplyQueue,
                               &dmqTmp.dmqclass, &dmqTmp.dmqtype, &bufferSize,
                               &buffLen, (int32 *) &dmqRec->filter, &dmqTmp.psb,
                               0, 0, 0, 0, 0);
  } while (pamsStatus == PAMS__SUCCESS);
    
  return;
}

/************************ End of function TLK_DMQ_Flush ***********************/


/*******************************************************************************

   TLK_DMQ_Confirm:                                   Date:  July 1994
   ~~~~~~~~~~~~~~~~                                 Author:  Sebastien Spicer

   This function will confirm receipt of a recoverable message.

      Arguments:
      ~~~~~~~~~~
         - dmqRec (type is *TLK_dmqRec - Read/Write):
              The referenced structure contains fields for communicating with
              the associated queue.  Some fields (for internal use) may be
              modified.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_Confirm (TLK_dmqRec *dmqRec) {

  long int pcjStatus;
  long int pamsStatus;
  char     forceJ;
  
  /*..........................................................................*/
  /* Verify that a recoverable message was received...                        */
  
  if ((dmqRec->psb.del_psb_status != PAMS__CONFIRMREQ) &&
      (dmqRec->psb.del_psb_status != PAMS__POSSDUPL))
    return (_RET_SUCCESS);
  
  /*..........................................................................*/
  /* Proceed to confirm receipt of recoverable message...                     */
  
  forceJ = PDEL_NO_JRN;
  pcjStatus = 0;
#ifdef VMS
  pamsStatus = pams_confirm_msg ( dmqRec->psb.seq_number,
                                  (int32 *) &pcjStatus, &forceJ);
#else
  pamsStatus = pams_confirm_msg ( dmqRec->psb.seq_number,
                                  (int32 *) &pcjStatus, &forceJ);
#endif
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  return (TLK_DMQ_SetError ("PAMS_CONFIRM_MSG", pamsStatus));
}

/*********************** End of function TLK_DMQ_Confirm **********************/


/*******************************************************************************

   TLK_DMQ_SetError:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will set an alarm according to the given PAMS return code.

      Arguments:
      ~~~~~~~~~~
         - mesg (type is *char - Read only):
              The referenced string refers to the calling function where the
              error occured.

         - status (type is long - Read only):
              The coded value is a DMQ specific return code.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.  If a DMQ error was signaled, then _RET_DMQERROR is
         returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_SetError (char *mesg,
                      long status) {

  char errorMsg[_SIZ_GENFIELD];
  char temp[_SIZ_GENFIELD];

  sprintf (temp, "DMQ error %ld", status);
  
  switch (status) {
  case PAMS__SUCCESS:
    return (_RET_SUCCESS);
    break;

  case PAMS__TIMEOUT:
    return (TRC_SetAlarm (mesg, _RET_TIMEOUT, message.host));
    break;
    
  case PAMS__DECLARED:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_DECLARED, _PAMS_MSG_DECLARED);
    break;
    
  case PAMS__NOOBJECT:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOOBJECT, _PAMS_MSG_NOOBJECT);
    break;
    
  case PAMS__RESRCFAIL: 
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_RESRCFAIL, _PAMS_MSG_RESRCFAIL);
    break;
    
  case PAMS__NOTACTIVE:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTACTIVE, _PAMS_MSG_NOTACTIVE);
    break;
    
  case PAMS__PAMSDOWN:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_PAMSDOWN, _PAMS_MSG_PAMSDOWN);
    break;
    
  case PAMS__NOTDCL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTDCL, _PAMS_MSG_NOTDCL);
    break;
    
  case PAMS__EXCEEDQUOTA:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_EXCEEDQUOTA, _PAMS_MSG_EXCEEDQUOTA);
    break;
    
  case PAMS__EXHAUSTBLKS:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_EXHAUSTBLKS, _PAMS_MSG_EXHAUSTBLKS);
    break;
    
  case PAMS__REMQUEFAIL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_REMQUEFAIL, _PAMS_MSG_REMQUEFAIL);
    break;
    
  case PAMS__NETNOLINK:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NETNOLINK, _PAMS_MSG_NETNOLINK);
    break;
    
  case PAMS__NETLINKLOST:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NETLINKLOST, _PAMS_MSG_NETLINKLOST);
    break;
    
  case PAMS__AREATOSMALL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_AREATOSMALL, _PAMS_MSG_AREATOSMALL);
    break;
    

  case PAMS__INSQUEFAIL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INSQUEFAIL, _PAMS_MSG_INSQUEFAIL);
    break;
    
  case PAMS__BADPARAM:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_BADPARAM, _PAMS_MSG_BADPARAM);
    break;
    
  case PAMS__INVALIDNUM:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INVALIDNUM, _PAMS_MSG_INVALIDNUM);
    break;
    
  case PAMS__INVFORMAT:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INVFORMAT, _PAMS_MSG_INVFORMAT);
    break;
    
  case PAMS__BADSELIDX:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_BADSELIDX, _PAMS_MSG_BADSELIDX);
    break;
    
  case PAMS__IDXTBLFULL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_IDXTBLFULL, _PAMS_MSG_IDXTBLFULL);
    break;
    
  case PAMS__NOTSUPPORTED:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTSUPPORTED, _PAMS_MSG_NOTSUPPORTED);
    break;
    
  case PAMS__NO_DQF:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NO_DQF, _PAMS_MSG_NO_DQF);
    break;
    
  default:
    sprintf (errorMsg, "%s %s", temp, _PAMS_MSG_DEFAULT);
    break;
  }

  return (TRC_SetAlarm (mesg, _RET_DMQERROR, errorMsg));
}

/********************** End of function TLK_DMQ_SetError **********************/


/*******************************************************************************

   TLK_DMQ_LocQueue:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function is used when locating an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - ident (type is *int - Read only):
              The referenced structure contains the key record for which the
              search is being performed.

         - elem (type is *TLK_dmqRec - Read only):
              The referenced structure contains an element to be compared
              with the identifier.

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when identifier matches
         element.  The function returns 1 when identifier is greater than
         element and -1 when identifier is less than element.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_LocQueue (int        *ident,
                      TLK_dmqRec *elem) {

  if (*ident > elem->handle)
    return (1);
  if (*ident < elem->handle)
    return (-1);
  return (0);
}

/********************** End of function TLK_DMQ_LocQueue **********************/


/*******************************************************************************

   TLK_DMQ_CmpQueue:                                 Date:  July 1996
   ~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer

   This function is used when inserting an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - elem1 (type is *TLK_dmqRec - Read only):
              The referenced structure contains an element to be compared
              with elem2.

         - elem2 (type is *TLK_dmqRec - Read only):
              The referenced structure contains an element to be compared
              with elem1

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when the two elements are 
         equal.  The function returns 1 when elem1 is greater than elem2 and
         -1 when elem1 is less than elem2.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_CmpQueue (TLK_dmqRec *elem1,
                      TLK_dmqRec *elem2) {



  if (elem1->handle > elem2->handle)
    return (1);
  if (elem1->handle < elem2->handle)
    return (-1);
  return (0);
}

/********************** End of function TLK_DMQ_CmpQueue **********************/


/*******************************************************************************

   TLK_DMQ_LocQTgt:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function is used when locating an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - ident (type is *int - Read only):
              The referenced structure contains the key record for which the
              search is being performed.

         - elem (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with the identifier.

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when identifier matches
         element.  The function returns 1 when identifier is greater than
         element and -1 when identifier is less than element.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_LocQTgt (int        *ident,
                     TLK_dmqTgt *elem) {

  if (*ident > elem->target)
    return (1);
  if (*ident < elem->target)
    return (-1);
  return (0);
}

/********************** End of function TLK_DMQ_LocQTgt ***********************/


/*******************************************************************************

   TLK_DMQ_CmpQTgt:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function is used when inserting an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - elem1 (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with elem2.

         - elem2 (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with elem1

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when the two elements are 
         equal.  The function returns 1 when elem1 is greater than elem2 and
         -1 when elem1 is less than elem2.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_DMQ_CmpQTgt (TLK_dmqTgt *elem1,
                     TLK_dmqTgt *elem2) {

 
  if (elem1->target > elem2->target)
    return (1);
  if (elem1->target < elem2->target)
    return (-1);
  return (0);
}

/*********************** End of function TLK_DMQ_CmpQTgt **********************/



/*******************************************************************************



   TLK_LogOutMsg:                                       Date:  April 1996

   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer



  


*******************************************************************************/



void  TLK_LogOutMsg (char *logFile,int  verbose) 
{

  char field[_SIZ_LOGBLOCK + 1];

  int parse;

  if (TRC_GetVerb (logFile) < verbose) return;

  TRC_Send (logFile, verbose, _TRC_OUTSTR, TRC_GetTime (),

            message.msgLen, message.host);

  if (TRC_GetVerb (logFile) < (verbose + 1)) return;

  parse = 0;

  while (parse < message.msgLen) {

    strncpy (field, &message.buffer[parse], _SIZ_LOGBLOCK);

    field[_SIZ_LOGBLOCK] = 0;

    TRC_Send (logFile, verbose, "  >%-74s<\n", field);

    parse += _SIZ_LOGBLOCK;

  }

  

  return;

}



/********************* End of function TLK_LogOutMsg **********************/





/*******************************************************************************



   TLK_DMQ_LogInpMsg:                                 Date:  April 1996

   ~~~~~~~~~~~~~~~~~~                               Author:  Sebastien Spicer


*******************************************************************************/



void  TLK_LogInpMsg (char *logFile,int  verbose) 
{



  char field[_SIZ_LOGBLOCK + 1];

  int parse;



  if (TRC_GetVerb (logFile) < verbose) return;

  TRC_Send (logFile, verbose, _TRC_INPSTR, TRC_GetTime (),

            message.msgLen, message.host);



  if (TRC_GetVerb (logFile) < (verbose + 1)) return;

  parse = 0;

  while (parse < message.msgLen) {

    strncpy (field, &message.buffer[parse], _SIZ_LOGBLOCK);

    field[_SIZ_LOGBLOCK] = 0;

    TRC_Send (logFile, verbose, "  >%-74s<\n", field);

    parse += _SIZ_LOGBLOCK;

  }

  

  return;

}



/********************* End of function TLK_LogInpMsg **********************/



/*******************************************************************************

   TLK_Flush:                                     Date:  July 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will flush a queue of pending messages.

      Arguments:
      ~~~~~~~~~~
         - dmqRec (type is *TLK_dmqRec - Read/Write):
              The referenced structure contains fields for communicating with
              the associated queue.  Some fields (for internal use) may be
              modified.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  TLK_Flush (TLK_dmqRec *dmqRec) {

  TLK_dmqRec dmqTmp;
  long int   pamsStatus;
  short int  buffLen;
  short      bufferSize = _SIZ_MSGBUFF;
  char       buffer[_SIZ_MSGBUFF];
  char       priority = 0;

  /*..........................................................................*/
  /* Exit if no queue is defined...                                           */

  if (dmqRec == NULL)
    return;
  memset (&dmqTmp, 0, sizeof (TLK_dmqRec));

  /*..........................................................................*/
  /* Loop until all messages are gone (or error)...                           */

  do {
    pamsStatus = pams_get_msg (buffer, &priority, &dmqTmp.rplyQueue,
                               &dmqTmp.dmqclass, &dmqTmp.dmqtype, &bufferSize,
                               &buffLen, (int32 *) &dmqRec->filter, &dmqTmp.psb,
                               0, 0, 0, 0, 0);
  } while (pamsStatus == PAMS__SUCCESS);
    
  return;
}

/************************ End of function TLK_Flush ***********************/


/*******************************************************************************

   TLK_SetError:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function will set an alarm according to the given PAMS return code.

      Arguments:
      ~~~~~~~~~~
         - mesg (type is *char - Read only):
              The referenced string refers to the calling function where the
              error occured.

         - status (type is long - Read only):
              The coded value is a DMQ specific return code.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.  If a DMQ error was signaled, then _RET_DMQERROR is
         returned.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_SetError (char *mesg,
                      long status) {

  char errorMsg[_SIZ_GENFIELD];
  char temp[_SIZ_GENFIELD];

  sprintf (temp, "DMQ error %ld", status);
  
  switch (status) {
  case PAMS__SUCCESS:
    return (_RET_SUCCESS);
    break;

  case PAMS__TIMEOUT:
    return (TRC_SetAlarm (mesg, _RET_TIMEOUT, message.host));
    break;
    
  case PAMS__DECLARED:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_DECLARED, _PAMS_MSG_DECLARED);
    break;
    
  case PAMS__NOOBJECT:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOOBJECT, _PAMS_MSG_NOOBJECT);
    break;
    
  case PAMS__RESRCFAIL: 
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_RESRCFAIL, _PAMS_MSG_RESRCFAIL);
    break;
    
  case PAMS__NOTACTIVE:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTACTIVE, _PAMS_MSG_NOTACTIVE);
    break;
    
  case PAMS__PAMSDOWN:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_PAMSDOWN, _PAMS_MSG_PAMSDOWN);
    break;
    
  case PAMS__NOTDCL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTDCL, _PAMS_MSG_NOTDCL);
    break;
    
  case PAMS__EXCEEDQUOTA:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_EXCEEDQUOTA, _PAMS_MSG_EXCEEDQUOTA);
    break;
    
  case PAMS__EXHAUSTBLKS:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_EXHAUSTBLKS, _PAMS_MSG_EXHAUSTBLKS);
    break;
    
  case PAMS__REMQUEFAIL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_REMQUEFAIL, _PAMS_MSG_REMQUEFAIL);
    break;
    
  case PAMS__NETNOLINK:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NETNOLINK, _PAMS_MSG_NETNOLINK);
    break;
    
  case PAMS__NETLINKLOST:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NETLINKLOST, _PAMS_MSG_NETLINKLOST);
    break;
    
  case PAMS__AREATOSMALL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_AREATOSMALL, _PAMS_MSG_AREATOSMALL);
    break;
    

  case PAMS__INSQUEFAIL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INSQUEFAIL, _PAMS_MSG_INSQUEFAIL);
    break;
    
  case PAMS__BADPARAM:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_BADPARAM, _PAMS_MSG_BADPARAM);
    break;
    
  case PAMS__INVALIDNUM:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INVALIDNUM, _PAMS_MSG_INVALIDNUM);
    break;
    
  case PAMS__INVFORMAT:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_INVFORMAT, _PAMS_MSG_INVFORMAT);
    break;
    
  case PAMS__BADSELIDX:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_BADSELIDX, _PAMS_MSG_BADSELIDX);
    break;
    
  case PAMS__IDXTBLFULL:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_IDXTBLFULL, _PAMS_MSG_IDXTBLFULL);
    break;
    
  case PAMS__NOTSUPPORTED:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NOTSUPPORTED, _PAMS_MSG_NOTSUPPORTED);
    break;
    
  case PAMS__NO_DQF:
    sprintf (errorMsg, "%s (%s) %s", temp,
             _PAMS_STR_NO_DQF, _PAMS_MSG_NO_DQF);
    break;
    
  default:
    sprintf (errorMsg, "%s %s", temp, _PAMS_MSG_DEFAULT);
    break;
  }

  return (TRC_SetAlarm (mesg, _RET_DMQERROR, errorMsg));
}

/********************** End of function TLK_SetError **********************/



/*******************************************************************************

   TLK_LocQTgt:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function is used when locating an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - ident (type is *int - Read only):
              The referenced structure contains the key record for which the
              search is being performed.

         - elem (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with the identifier.

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when identifier matches
         element.  The function returns 1 when identifier is greater than
         element and -1 when identifier is less than element.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  TLK_LocQTgt (int        *ident,
                     TLK_dmqTgt *elem) {

  if (*ident > elem->target)
    return (1);
  if (*ident < elem->target)
    return (-1);
  return (0);
}

/********************** End of function TLK_LocQTgt ***********************/


/*******************************************************************************

   TLK_CmpQTgt:                                  Date:  July 1996
   ~~~~~~~~~~~~~~~~                                Author:  Sebastien Spicer

   This function is used when inserting an element in an ordered array.

      Arguments:
      ~~~~~~~~~~
         - elem1 (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with elem2.

         - elem2 (type is *TLK_dmqTgt - Read only):
              The referenced structure contains an element to be compared
              with elem1

      Returns:
      ~~~~~~~~
         Type is int - the return value will be 0 when the two elements are 
         equal.  The function returns 1 when elem1 is greater than elem2 and
         -1 when elem1 is less than elem2.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_CmpQTgt (TLK_dmqTgt *elem1,
                     TLK_dmqTgt *elem2) {

  if (elem1->target > elem2->target)
    return (1);
  if (elem1->target < elem2->target)
    return (-1);
  return (0);
}

/*********************** End of function TLK_CmpQTgt **********************/
int TLK_DMQ_CreateTimer( int *TimerId, int TimeInSec )
{

        int32 status,Ptimeout;
        long int Stimeout=0;      /* Not used - Only for OPEN VMS- Format = 'S' */
        char timerFormat = 'P';   /* Ineterval Timer */

        /*..........................................................................*/
        
        if ( TimeInSec <=0 ) return _RET_FAILURE;
        Ptimeout=TimeInSec * 10;

        status=pams_set_timer(&DmqTimerId,&timerFormat,
                                        &Ptimeout, (int32 *) &Stimeout); 
        *TimerId = DmqTimerId;
        
        ++DmqTimerId;

        if (status!= PAMS__SUCCESS)
                        return(_RET_FAILURE);
        return(_RET_SUCCESS);

  } 
/*******************************************************************************

   TLK_DMQ_ClearTimer:                         Date:May  1997
   ~~~~~~~~~~~~~~~~~~~                              Author: Nawal Parwal

   This function will clear a DMQ timer identified by timer_id.

      Arguments:
      ~~~~~~~~~~
         - timer_id (type is int - Read only):
              The referenced code is the handle to be timer to be cleared
                  
         
      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int TLK_DMQ_ClearTimer (int32 *timer_id)
{

        int32 status;
  /*..........................................................................*/
        if ((status =pams_cancel_timer(timer_id)) == PAMS__SUCCESS)
                return (_RET_SUCCESS);
        else
                return (status);
}

/********************* End of function TLK_ClearAsyncTimer *********************/

int TLK_DMQ_TimeOut ()
{
        return(  ExtRecdTimerId );
}

/********************* End of function TLK_ClearAsyncTimer *********************/

/************************************************************************************/
/*   The following portion has VB related DMQ  routines used for DLL                 */
/************************************************************************************/
#ifdef WINNT

/*******************************************************************************

   TLK_DMQ_VB_SetQueue                                    Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_SetQueue (int  handle,BSTR qName,
                      long int attMode,BSTR logFile,int  verbose) 
{
        char locHostName[_SIZ_GENFIELD],locLogFile[_SIZ_GENFIELD];

        
        if (  qName == NULL ) 
                strcpy ( locHostName,"");
        else
                strcpy ( locHostName,(char *)qName);

        if ( logFile == NULL ) 
        {
                return ( TRC_SetAlarm ("TLK_DMQ_VB_SetTarget",_RET_GENERIC,
                        "Null Value for Logfile ALias"));
        }
        strcpy ( locLogFile,(char *)logFile);
        
        return ( TLK_DMQ_SetQueue (handle,locHostName,attMode,locLogFile,verbose) );
}
/*******************************************************************************

   TLK_DMQ_VB_GetQDetail                              Date:  May'98
   ~~~~~~~~~~~~                                    Author:  Nawal

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_GetQDetail (int target,int  *GroupId,
                       int  *QueueId,BSTR logFile,int  verbose)
{
        char locLogFile[_SIZ_GENFIELD];
        int grp ,que,sts;
        
        if ( logFile == NULL ) 
        {
                return ( TRC_SetAlarm ("TLK_DMQ_VB_GetQDetail",_RET_GENERIC,
                        "Null Value for Logfile ALias"));
        }
        strcpy ( locLogFile,(char *)logFile);
        
        sts= TLK_DMQ_GetQDetail (target,&grp,&que,
                       locLogFile,  verbose);
        
        if (sts==_RET_SUCCESS){
                *GroupId=grp;
                *QueueId=que;
        }
}
/*******************************************************************************/
/*******************************************************************************

   TLK_DMQ_VB_SetTarget                              Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_SetTarget (int  target,BSTR hostName,int  rplyHandle,
                       int  timeout,BSTR logFile,int  verbose)
{
        char locHostName[_SIZ_GENFIELD],locLogFile[_SIZ_GENFIELD];

        
        if (  hostName == NULL ) 
        {
                return ( TRC_SetAlarm ("TLK_DMQ_VB_SetTarget",_RET_GENERIC,
                        "Null Value for Target Queue "));
        }
        if ( logFile == NULL ) 
        {
                return ( TRC_SetAlarm ("TLK_DMQ_VB_SetTarget",_RET_GENERIC,
                        "Null Value for Logfile ALias"));
        }
        strcpy ( locLogFile,(char *)logFile);
        strcpy ( locHostName,(char *)hostName);
        return (  TLK_DMQ_SetTarget (target,locHostName,rplyHandle,
                       timeout, locLogFile,  verbose));
}
/*******************************************************************************

   TLK_DMQ_VB_SetTargetQnumber                     Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_SetTargetQNumber (int  target,int group,int queue,int  rplyHandle,
                       int  timeout,BSTR logFile,int  verbose)
{
        char locLogFile[_SIZ_GENFIELD];

        
        if ( logFile == NULL ) 
        {
                return ( TRC_SetAlarm ("TLK_DMQ_VB_SetTarget",_RET_GENERIC,
                        "Null Value for Logfile ALias"));
        }
        strcpy ( locLogFile,(char *)logFile);
        return (  TLK_DMQ_SetTargetQNumber (target,group,queue,rplyHandle,
                       timeout, locLogFile,  verbose));
}
/*******************************************************************************

   TLK_DMQ_VB_ClientPut                                    Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_ClientPut (int target)
{
        return ( TLK_DMQ_ClientPut (target) );
}
/*******************************************************************************

   TLK_DMQ_VB_ClientGet                                    Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_ClientGet (int target)
{
        return ( TLK_DMQ_ClientGet (target) );
}
/*******************************************************************************

   TLK_DMQ_VB_Shutdown                                    Date:  Apr'98
   ~~~~~~~~~~~~                                                                                 Author:  Nawal

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void  WINNTAPI TLK_DMQ_VB_Shutdown (void)
{
        TLK_DMQ_Shutdown () ;
}
/*******************************************************************************


   TLK_DMQ_VB_ServerGet                            Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_ServerGet (int  handle,
                       int  msgType, BSTR *msgstr)
{

        char lmsg[_SIZ_MSGBUFF];

        

        TLK_DMQ_ServerGet(handle,msgType,lmsg);
        if ( TRC_ChkAlarm() )
        {
                *msgstr = SysAllocStringByteLen((char *)lmsg,strlen(lmsg) );
                
        }
                
        return ( TRC_GetAlarm() );
        
        
}
/*******************************************************************************

   TLK_DMQ_VB_ServerGetNoWai                         Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_ServerGetNoWait (int  handle,int  msgType)
{
        return ( TLK_DMQ_ServerGetNoWait(handle,msgType) );
}                   

/*******************************************************************************

   TLK_DMQ_VB_ServerPut                                    Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

int  WINNTAPI TLK_DMQ_VB_ServerPut (int  handle) 
{
        return(TLK_DMQ_ServerPut (handle) );
}
/*******************************************************************************

   TLK_VB_LogOutMsg                                  Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void WINNTAPI TLK_VB_LogOutMsg ( BSTR logFile,int  verbose)
{
        char lLogFile[_SIZ_GENFIELD];

        
        if (  logFile == NULL ) 
        {
                TRC_SetAlarm ("TLK_VB_LogOutMsg",_RET_GENERIC,
                        "Null Value for LogFile Alias ");
                return;
        }

        strcpy ( lLogFile,(char *)logFile);
        TLK_LogOutMsg (lLogFile,  verbose);
        return;
}
/*******************************************************************************

   TRC_VB_LogInpMsg                                   Date:  Apr'98
   ~~~~~~~~~~~~                                    Author:  Jaya

   
      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
           Description...

*******************************************************************************/

void WINNTAPI TLK_VB_LogInpMsg (BSTR logFile,int  verbose)
{
        char lLogFile[_SIZ_GENFIELD];

        
        if (  logFile == NULL ) 
        {
                 TRC_SetAlarm ("TLK_VB_LogInpMsg",_RET_GENERIC,
                        "Null Value for LogFile Alias ");
                 return;
        }

        strcpy ( lLogFile,(char *)logFile);
        
        TLK_LogInpMsg (lLogFile,  verbose) ;
        return;
}
#endif

/******************************************************************************/
/*************************** End of file talkDMQ.c ****************************/
/******************************************************************************/


