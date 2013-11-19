/******************************************************************************/
/*                                                                            */
/*   dmqutil.c:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*    This file contains functions that are required to setup and use DMQ.    */
/*    The functions contained here remove DMQ details from the calling        */
/*    function's scope.  The calling program only needs to remember the       */
/*    name and attach mode of the queue.                                      */
/*                                                                            */
/******************************************************************************/


/*****************************************************************************/
/* The following are standard C include statements.                          */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

/* End of standard C includes.                                                */
/******************************************************************************/


/******************************************************************************/
/* The following are include statements for local files.                      */

#include "dmqutil.h"

/* End of include statements for local files.                                 */
/******************************************************************************/


/******************************************************************************/
/* Declaration of global variables with file scope.                           */

int  DMQ_IsUp = 0;
char DMQ_ErrorMsg[_DMQ_FLDSZ];

/* End of global variable declarations...                                     */
/******************************************************************************/


/*******************************************************************************

   DMQ_Shutdown:                                      Date:  November 1994
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function shuts down DMQ for the application.

      Arguments:
      ~~~~~~~~~~
         None.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_Shutdown (void) {

  long int pamsStatus;

  /*..........................................................................*/
  /* Shutdown DECmessageQ...                                                  */

  if (DMQ_IsUp != 0)
    pamsStatus = pams_exit ();
  DMQ_IsUp = 0;
  return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_Shutdown ***********************/


/*******************************************************************************

   DMQ_ServerSet:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will setup a server type queue.  It will attach the
   application to the named queue.  It will also set the queue parameters
   for server type operations.  If the queue is the first to be attached,
   it will become the primary queue.  Subsequent queues will be attached
   as secondary queues.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the specified queue.  They will be set accordingly.

	 - name (type is *char - Read only):
              The referenced string contains either the queue name or number
	      (depending on the value of attMode).  If the attach mode is
	      temporary, this field is used internally only.

	 - attMode (type is long int - Read only):
              The value of attMode represent the attach mode to be used by
	      DECmessageQ.  This field can be one of PSYM_ATTACH_BY_NAME,
	      PSYM_ATTACH_BY_NUMBER or PSYM_ATTACH_TEMPORARY.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ServerSet (DMQ_parms *dmq,
		   char      *name,
		   long int  attMode) {

  int status;

  /*..........................................................................*/
  /* Setup server specific parameters and call generic function...            */

  dmq->qType = _DMQ_SERVER;
  dmq->get.timeout = 0;
  status = DMQ_QueueSet (dmq, name, attMode);
  return (status);
}

/************************* End of function DMQ_ServerSet **********************/


/*******************************************************************************

   DMQ_ServerGet:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will get a message from DMQ for a server type queue.  The
   get will only select the queue represented by the "dmq" structure.
   Data is returned in the "packet" structure.  The function will wait
   indefinitely for the reply (unless the value for timeout is modified).

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read/Write):
              The referenced structure contains fields that will be filled
	      with the query data from the client.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ServerGet (DMQ_parms *dmq,
		   DMQ_pckt  *packet) {

  int status;

  /*..........................................................................*/
  /* Make sure that the queue can be read...                                  */

  if (dmq->qType != _DMQ_SERVER)
    return (_DMQ_WRONGQ);

  /*..........................................................................*/
  /* Receive data from queue...                                               */

  if ((status = DMQ_Receive (dmq, packet)) == _DMQ_SUCCESS) {
    dmq->mClass = packet->class;
    dmq->mType = packet->type;
  }
  return (status);
}

/************************* End of function DMQ_ServerGet **********************/


/*******************************************************************************

   DMQ_ServerPut:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will put a message to DMQ for a server type queue.  The
   put is targeted at the response queue defined in the previous get.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read/Write):
              The referenced structure contains fields that are expected to
	      contain data to be sent to the client.  The packet may be
	      emptied if the original message does not require a reply.

	 - xCode (type is int - Read only):
              The coded value indicates if the server successfuly completed
	      the transaction.  If an error occured, xCode should be non
	      zero (recoverable messages will not be confirmed).

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ServerPut (DMQ_parms *dmq,
		   DMQ_pckt  *packet,
		   int       xCode) {

  int status;

  /*..........................................................................*/
  /* Make sure that the queue can be written to...                            */

  if (dmq->qType != _DMQ_SERVER)
    return (_DMQ_WRONGQ);

  /*..........................................................................*/
  /* Confirm that recoverable message was successfuly processed...            */

  if (xCode == 0)
    if ((status = DMQ_Confirm (dmq)) != _DMQ_SUCCESS)
      return (status);

  /*..........................................................................*/
  /* If no reply is requested, do not send it out...                          */

  if ((dmq->mType == _DMQ_TYP_QUIET) || (dmq->mType == _DMQ_TYP_RECOV)) {
    packet->buffLen = 0;
    packet->buffPtr = 0;
    return (_DMQ_SUCCESS);
  }

  /*..........................................................................*/
  /* Send data to queue...                                                    */
  
  dmq->put.target.all = dmq->get.source.all;
  return (DMQ_Submit (dmq, packet));
}

/************************* End of function DMQ_ServerPut **********************/


/*******************************************************************************

   DMQ_ClientSet:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will setup a client type queue.  It will attach the
   application to the named queue.  It will also set the queue parameters
   for client type operations.  If the queue is the first to be attached,
   it will become the primary queue.  Subsequent queues will be attached
   as secondary queues.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the specified queue.  They will be set accordingly.

	 - name (type is *char - Read only):
              The referenced string contains either the queue name or number
	      (depending on the value of attMode).  If the attach mode is
	      temporary, this field is used internally only.

	 - attMode (type is long int - Read only):
              The value of attMode represent the attach mode to be used by
	      DECmessageQ.  This field can be one of PSYM_ATTACH_BY_NAME,
	      PSYM_ATTACH_BY_NUMBER or PSYM_ATTACH_TEMPORARY.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ClientSet (DMQ_parms *dmq,
		   char      *name,
		   long int  attMode,
		   int *timeout)
{

  int status;

  /*..........................................................................*/
  /* Setup server specific parameters and call generic function...            */

  dmq->qType = _DMQ_CLIENT;
  dmq->get.timeout = (*timeout) * 10 ;   /* Multiply by 10 to  1/10 of sec */
  return (DMQ_QueueSet (dmq, name, attMode));
}

/************************* End of function DMQ_ClientSet **********************/


/*******************************************************************************

   DMQ_ClientSrv:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will locate a server queue and setup the client queue so that
   subsequent querries will address the specified server.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the specified queue.  They will be set accordingly.

	 - name (type is *char - Read only):
              The referenced string contains either the queue name of the
	      server to be linked to.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ClientSrv (DMQ_parms *dmq,
		   char      *name) {

  long int pamsStatus;
  int32 nameLen;         /* pang */
  long int waitMode;
  long int searchListLen = 4;
  long int searchList[4] = {PSEL_TBL_PROC, PSEL_TBL_GRP, 
			    PSEL_TBL_DNS_CACHE, PSEL_TBL_DNS_LOW};

  /*..........................................................................*/
  /* Attempt to locate server queue...                                        */

  nameLen = strlen (name);
  waitMode = PSYM_WF_RESP;
#ifdef VMS
  pamsStatus = pams_locate_q (name, &nameLen, (q_address *)&dmq->put.target.all,
			      0, 0, 0, 0, 0, 0);      /* pang */
#else
  pamsStatus = pams_locate_q (name, &nameLen, &dmq->put.target,
			      &waitMode, NULL, NULL, &searchList[1],
			      &searchListLen, NULL);
#endif
  if (pamsStatus != PAMS__SUCCESS) {
    DMQ_Error ("PAMS_LOCATE_Q", pamsStatus);
    return (_DMQ_NOSERVER);
  } else 
    return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_ClientSrv **********************/


/*******************************************************************************

   DMQ_ClientGet:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will get a message from DMQ for a client type queue.  The
   get will only select the queue represented by the "dmq" structure.
   Data is returned in the "packet" structure.  The function will wait
   for the reply until the timeout counter has expired.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read/Write):
              The referenced structure contains fields that will be filled
	      with the reply data from the server.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ClientGet (DMQ_parms *dmq,
		   DMQ_pckt  *packet) {

  /*..........................................................................*/
  /* Make sure that the queue can be read...                                  */

  if (dmq->qType != _DMQ_CLIENT)
    return (_DMQ_WRONGQ);
  
  /*..........................................................................*/
  /* If no reply is requested, do not try to receive it...                    */

  if ((dmq->mType == _DMQ_TYP_QUIET) || (dmq->mType == _DMQ_TYP_RECOV)) {
    packet->buffLen = 0;
    packet->buffPtr = 0;
    return (_DMQ_SUCCESS);
  }

  /*..........................................................................*/
  /* Get data from queue...                                                   */

  return (DMQ_Receive (dmq, packet));
}

/************************* End of function DMQ_ClientGet **********************/


/*******************************************************************************

   DMQ_ClientPut:                                     Date:  November 1994
   ~~~~~~~~~~~~~~                                   Author:  Sebastien Spicer

   This function will put a message to DMQ for a client type queue.  The
   put is targeted at the server queue defined by the last call to the
   DMQ_ClientSrv function.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read only):
              The referenced structure contains fields that are expected to
	      contain the data to be sent by the client.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_ClientPut (DMQ_parms *dmq,
		   DMQ_pckt  *packet) {

  int status;

  /*..........................................................................*/
  /* Make sure that the queue can be written to...                            */

  if (dmq->qType != _DMQ_CLIENT)
    return (_DMQ_WRONGQ);

  /*..........................................................................*/
  /* Send data to queue...                                                    */

  if ((status = DMQ_Submit (dmq, packet)) == _DMQ_SUCCESS) {
    dmq->mClass = packet->class;
    dmq->mType = packet->type;
  }
  return (status);
}

/************************* End of function DMQ_ClientPut **********************/


/*******************************************************************************

   DMQ_QueueSet:                                      Date:  November 1994
   ~~~~~~~~~~~~~                                    Author:  Sebastien Spicer

   This function will setup and attach to a queue.  It should be called
   by either DMQ_ServerSet or DMQClient_Set.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the specified queue.  They will be set accordingly.

	 - name (type is *char - Read only):
              The referenced string contains either the queue name or number
	      (depending on the value of attMode).  If the attach mode is
	      temporary, this field is used internally only.

	 - attMode (type is long int - Read only):
              The value of attMode represent the attach mode to be used by
	      DECmessageQ.  This field can be one of PSYM_ATTACH_BY_NAME,
	      PSYM_ATTACH_BY_NUMBER or PSYM_ATTACH_TEMPORARY.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_QueueSet (DMQ_parms *dmq,
		  char      *name,
		  long int  attMode) {

  int status;

  /*..........................................................................*/
  /* Initialize general queue parameters...                                   */

  strcpy (dmq->name, name);
  dmq->attachMode = attMode;
  
  /*..........................................................................*/
  /* Initialize parameters related to "get"...                                */
  
  dmq->get.priority = 0;
  
  /*..........................................................................*/
  /* Initialize parameters related to "put"...                                */
  
  dmq->put.priority = 0;
  dmq->put.timeout = 0;
  
  /*..........................................................................*/
  /* Attach to named queue and set selection handle...                        */
  
  if ((status = DMQ_AttachQ (dmq)) == _DMQ_SUCCESS)
    DMQ_IsUp = 1;
  return (status);
}

/************************* End of function DMQ_QueueSet ***********************/


/*******************************************************************************

   DMQ_AttachQ:                                       Date:  November 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will attach the application to the queue described in the
   DMQ structure.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
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

int DMQ_AttachQ (DMQ_parms *dmq) {

  q_address selFilter;
  long int  pamsStatus;
  int32  nameLen;    /* pang */
  int       status;
  
  /*..........................................................................*/
  /* Attempt to attach to the queue provided in "dmq"...                      */
  
  nameLen = strlen (dmq->name);
  dmq->attachType = PSYM_ATTACH_PQ;
  if (dmq->attachMode != PSYM_ATTACH_TEMPORARY)
    pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
				&dmq->attachType, dmq->name, &nameLen,
				0, 0, 0, 0, 0);
  else {
    pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
				&dmq->attachType, 0, 0, 0, 0, 0, 0, 0);
    /* This is to fix the problem with queue 999 on VMS...                    */
    if (dmq->queue.au.queue == 999) {
      pamsStatus = pams_exit ();
      pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
                                  &dmq->attachType, 0, 0, 0, 0, 0, 0, 0);
    }
  }
  selFilter.au.group = PSEL_PQ;
  selFilter.au.queue = 0;
  dmq->get.select = *((long int *) &selFilter);
  dmq->put.respQue = dmq->queue;
  
  if (pamsStatus == PAMS__BADDECLARE) {
    dmq->attachType = PSYM_ATTACH_SQ;
    if (dmq->attachMode != PSYM_ATTACH_TEMPORARY)
      pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
				  &dmq->attachType, dmq->name, &nameLen,
				  0, 0, 0, 0, 0);
    else {
      pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
				  &dmq->attachType, 0, 0, 0, 0, 0, 0, 0);
      /* This is to fix the problem with queue 999 on VMS...                    */
      if (dmq->queue.au.queue == 999) {
        pamsStatus = pams_exit ();
        pamsStatus = pams_attach_q (&dmq->attachMode, &dmq->queue,
                                    &dmq->attachType, 0, 0, 0, 0, 0, 0, 0);
      }
    }
    selFilter.au.group = PSEL_AQ;
    selFilter.au.queue = dmq->queue.au.queue;
    dmq->get.select = *((long int *) &selFilter);
    dmq->put.respQue = dmq->queue;
  }
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  if (pamsStatus != PAMS__SUCCESS) {

    DMQ_Error ("PAMS_ATTACH_Q", pamsStatus);
    return (_DMQ_NOATTACH);
  } else
    return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_AttachQ ************************/


/*******************************************************************************

   DMQ_Receive:                                       Date:  November 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will wait for a message from DMQ.  It will read the message
   into the dmq read buffer.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read/Write):
              The referenced structure contains fields that will be filled
	      with data from the sending application.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_Receive (DMQ_parms *dmq,
		 DMQ_pckt  *packet) {

  long int pamsStatus;
  short    bufferSize;

  /*..........................................................................*/
  /* Attempt to retrieve a message from DMQ...                                */
  
  bufferSize = _DMQ_BUFFSZ;
  pamsStatus = pams_get_msgw (packet->buffer, &dmq->get.priority,
			      &dmq->get.source, &packet->class,
			      &packet->type, &bufferSize,
			      &packet->buffLen, &dmq->get.timeout,
			      &dmq->get.select, &dmq->get.psb,
			      0, 0, 0, 0, 0);
  packet->buffPtr = 0;
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  if (pamsStatus != PAMS__SUCCESS) {
    DMQ_Error ("PAMS_GET_MSGW", pamsStatus);
    if (pamsStatus == PAMS__TIMEOUT)
      return (_DMQ_TOUTERR);
    else
      return (_DMQ_NODMQGET);
  } else
    return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_Receive ************************/


/*******************************************************************************

   DMQ_Submit:                                        Date:  November 1994
   ~~~~~~~~~~~                                      Author:  Sebastien Spicer

   This function will submit a message through DMQ.  It submits the string 
   contained in the packet passed by the application.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
	      The referenced structure contains fields for communicating with
	      the associated queue.  Some fields (for internal use) may be
	      modified.

	 - packet (type is *DMQ_pckt - Read only):
              The referenced structure contains fields that are expected to
	      contain data to be sent to the target application.

      Returns:
      ~~~~~~~~
         Type is int - coded value representing the completion status of the
         function.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

int DMQ_Submit (DMQ_parms *dmq,
		DMQ_pckt  *packet) {

  long int pamsStatus;
  int      index;

  /*..........................................................................*/
  /* Set the delivery style for the message...                                */
  
  if (packet->type == _DMQ_TYP_RECOV) {
    dmq->put.delivery = PDEL_MODE_WF_DQF;
    dmq->put.uma = PDEL_UMA_SAF;
  } else {
    dmq->put.delivery = PDEL_MODE_WF_MEM;
    dmq->put.uma = PDEL_UMA_DISC;
  }
  
  /*..........................................................................*/
  /* Attempt to write message to DMQ...                                       */
  
  index = _DMQ_RETRY;
  do {
    pamsStatus = pams_put_msg (packet->buffer, &dmq->put.priority,
			       &dmq->put.target, &packet->class,
			       &packet->type, &dmq->put.delivery,
			       &packet->buffLen, &dmq->put.timeout,
			       &dmq->put.psb, &dmq->put.uma,
			       &dmq->put.respQue, 0, 0, 0);
  } while ((pamsStatus != PAMS__SUCCESS) && (--index != 0));
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  if (pamsStatus != PAMS__SUCCESS) {
    DMQ_Error ("PAMS_PUT_MSG", pamsStatus);
    if (pamsStatus == PAMS__TIMEOUT)
      return (_DMQ_TOUTERR);
    else
      return (_DMQ_NODMQPUT);
  } else
    return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_Submit *************************/


/*******************************************************************************

   DMQ_Confirm:                                       Date:  November 1994
   ~~~~~~~~~~~~                                     Author:  Sebastien Spicer

   This function will confirm receipt of a recoverable message.

      Arguments:
      ~~~~~~~~~~
         - dmq (type is *DMQ_parms - Read/Write):
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

int DMQ_Confirm (DMQ_parms *dmq) {

  int32 pcjStatus;           /* pang */
  long int pamsStatus;
  char     forceJ;
  
  /*..........................................................................*/
  /* Verify that a recoverable message was received...                        */
  
  if ((dmq->get.psb.del_psb_status != PAMS__CONFIRMREQ) &&
      (dmq->get.psb.del_psb_status != PAMS__POSSDUPL))
    return (_DMQ_SUCCESS);
  
  /*..........................................................................*/
  /* Proceed to confirm receipt of recoverable message...                     */
  
  forceJ = PDEL_NO_JRN;
  pcjStatus = 0;
#ifdef VMS
  pamsStatus = pams_confirm_msg ((uint32 *) &dmq->get.psb.seq_number,
				 &pcjStatus, &forceJ);        /* pang */
#else
  pamsStatus = pams_confirm_msg (dmq->get.psb.seq_number,
				 (int32 *) &pcjStatus, &forceJ);
#endif
  
  /*..........................................................................*/
  /* Report on the success status...                                          */
  
  if (pamsStatus != PAMS__SUCCESS) {
    DMQ_Error ("PAMS_CONFIRM_MSG", pamsStatus);
    return (_DMQ_NODMQCNF);
  } else
    return (_DMQ_SUCCESS);
}

/************************* End of function DMQ_Confirm ************************/


/*******************************************************************************

   DMQ_Error:                                         Date:  November 1994
   ~~~~~~~~~~                                       Author:  Sebastien Spicer

   This function load the error string with a message corresponding to a
   DMQ failure.

      Arguments:
      ~~~~~~~~~~
         - mesg (type is *char - Read only):
	      The referenced string refers to the calling function where the
	      error occured.

	 - status (type is long - Read only):
              The coded value is a DMQ specific return code.

      Returns:
      ~~~~~~~~
         Nothing.

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/

void DMQ_Error (char *mesg,
		long status) {

  char temp[_DMQ_FLDSZ];

  sprintf (temp, "Error %ld in %s", status, mesg);
  
  switch (status) {
  case PAMS__DECLARED:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_DECLARED, _PAMS_MSG_DECLARED);
    break;
    
  case PAMS__NOOBJECT:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NOOBJECT, _PAMS_MSG_NOOBJECT);
    break;
    
  case PAMS__RESRCFAIL: 
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_RESRCFAIL, _PAMS_MSG_RESRCFAIL);
    break;
    
  case PAMS__NOTACTIVE:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NOTACTIVE, _PAMS_MSG_NOTACTIVE);
    break;
    
  case PAMS__PAMSDOWN:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_PAMSDOWN, _PAMS_MSG_PAMSDOWN);
    break;
    
  case PAMS__TIMEOUT:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_TIMEOUT, _PAMS_MSG_TIMEOUT);
    break;
    
  case PAMS__NOTDCL:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NOTDCL, _PAMS_MSG_NOTDCL);
    break;
    
  case PAMS__EXCEEDQUOTA:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_EXCEEDQUOTA, _PAMS_MSG_EXCEEDQUOTA);
    break;
    
  case PAMS__EXHAUSTBLKS:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_EXHAUSTBLKS, _PAMS_MSG_EXHAUSTBLKS);
    break;
    
  case PAMS__REMQUEFAIL:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_REMQUEFAIL, _PAMS_MSG_REMQUEFAIL);
    break;
    
  case PAMS__NETNOLINK:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NETNOLINK, _PAMS_MSG_NETNOLINK);
    break;
    
  case PAMS__NETLINKLOST:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NETLINKLOST, _PAMS_MSG_NETLINKLOST);
    break;
    
  case PAMS__AREATOSMALL:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_AREATOSMALL, _PAMS_MSG_AREATOSMALL);
    break;
    
  case PAMS__INSQUEFAIL:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_INSQUEFAIL, _PAMS_MSG_INSQUEFAIL);
    break;
    
  case PAMS__BADPARAM:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_BADPARAM, _PAMS_MSG_BADPARAM);
    break;
    
  case PAMS__INVALIDNUM:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_INVALIDNUM, _PAMS_MSG_INVALIDNUM);
    break;
    
  case PAMS__INVFORMAT:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_INVFORMAT, _PAMS_MSG_INVFORMAT);
    break;
    
  case PAMS__BADSELIDX:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_BADSELIDX, _PAMS_MSG_BADSELIDX);
    break;
    
  case PAMS__IDXTBLFULL:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_IDXTBLFULL, _PAMS_MSG_IDXTBLFULL);
    break;
    
  case PAMS__NOTSUPPORTED:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NOTSUPPORTED, _PAMS_MSG_NOTSUPPORTED);
    break;
    
  case PAMS__NO_DQF:
    sprintf (DMQ_ErrorMsg, "%s (%s): %s.", temp,
	     _PAMS_STR_NO_DQF, _PAMS_MSG_NO_DQF);
    break;
    
  default:
    sprintf (DMQ_ErrorMsg, "%s: %s.", temp, _PAMS_MSG_DEFAULT);
    break;
  }
  return;
}

/************************* End of function DMQ_Error **************************/


/******************************************************************************/
/*************************** End of file dmqutil.c ****************************/
/******************************************************************************/
