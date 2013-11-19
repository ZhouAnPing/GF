/*******************************************************************************

   INIT_DECMSG_Q                                 Date:  FEB   1996
   ~~~~~~~~~~~~~                                 Author:Jayaraman Subramanian

   This function initializes the DEC MESSAGE Q 

      Arguments:
      ~~~~~~~~~~
         RETURN STATUS: This return the completion status of this
                        function: 0 Indicates success
                                  1 indicates failure

         ERROR MESSAGE: This variable contains the error message if the
                        return status is 1

      Returns:
      ~~~~~~~~
         NONE

      Modifications:
      ~~~~~~~~~~~~~~
         - Author                     Date
	   Description...

*******************************************************************************/
/*****************************************************************************/
/* The following are standard C include statements.                          */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/******************************************************************************/
/* The following are include statements for local files.                      */

#include "dmqutil.h"
#include "talkstd.h"
#include "trcutil.h"
#include "getmcs.h"

/******************************************************************************/
/* Declaration of global variables with file scope.                           */

DMQ_parms dmq;

void
init_decmsg_q(char *MsgQ,int *MsgQlen,int * Verbose, int *decq_timeout,
	       int* RetSts, char *ErrMsgC )
{
  int TmpVar;
  char NewMsgQ[255];

  for ( TmpVar=0;TmpVar<*MsgQlen;TmpVar++)
   NewMsgQ[TmpVar]=MsgQ[TmpVar];
  NewMsgQ[*MsgQlen]=(char) NULL;

  *RetSts=0;

  strcpy ( ErrMsgC,"INIT DEC MSG Q:SUCCESS");

  TRC_Open ("stdout", *Verbose);
  if ((DMQ_ClientSet(&dmq, "C3SYNC", PSYM_ATTACH_TEMPORARY,decq_timeout)
	!= _DMQ_SUCCESS) ||
      (DMQ_ClientSrv (&dmq, NewMsgQ ) != _DMQ_SUCCESS))
  {
    strcpy ( ErrMsgC, TRC_GetTime() );
    strcat ( ErrMsgC,DMQ_ErrorMsg);
    *RetSts=1;
  }
  return;
}
/****************************END OF FUNCTION INIT DEC MSG Q ************/
