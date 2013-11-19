/*******************************************************************************

   GET_MCS_INFO                                  Date:  FEB   1996
   ~~~~~~~~~~~~~                                 Author:Jayaraman Subramanian

   For a given list of lots, this functions tries to get equipment id
   and location from MCS system.

      Arguments:
      ~~~~~~~~~~
         number of lots:    Specifies the number of lots passed on to
                            this routine  ( input)


         lot id        :    List of lots for which eqp.id and location
                            will be requested.

         Location      :    Returns the Eqpid and Location id in this
                             variable back to calling function

        
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
get_mcs_info(int *num_lots,
              char LotId[][LOT_LEN],
              char McsLocation[][EQP_ID_LEN+LOC_ID_LEN+1],
              int *RetSts,
              char *ErrMsgC )
{

  DMQ_pckt packet;
  int TmpVar,RecvLots;
  char Ptr[80];
  char LotEqpId[EQP_ID_LEN+1];
  char LotLocId[LOC_ID_LEN+1];
  char TmpLotId[LOT_LEN+1];
  int LotIdPos;
  int EqpLen;


  *RetSts=0;

   strcpy ( ErrMsgC," GET MCS LOCATION :- SUCCESS");

/* Expect Reply from server */

  packet.type = _DMQ_TYP_REPLY;

/* Write Header.   Request is MCS_GET_LOT_ALL */

  TLK_PutMsgHeader (&packet, "MCS_GET_LOT_ALL",
	            "VMS MCS Client query",'|');

  sprintf (Ptr,"%d",*num_lots);

/* Number of lot ids */

  TLK_PutNextField (&packet, "%s", Ptr );

/* Array of Lot ids put to packet to be sent to server */

  for ( TmpVar =0; TmpVar < *num_lots ; TmpVar ++)
  {
     strncpy ( TmpLotId,LotId[TmpVar],LOT_LEN);
     TmpLotId[LOT_LEN]=(char)NULL;

     for ( LotIdPos=0; LotIdPos<LOT_LEN; LotIdPos++)
     {
       if ( TmpLotId[LotIdPos] == ' ' ) 
       {
         TmpLotId[LotIdPos] = (char) NULL;
         break;
       }
     }
     TLK_PutNextField (&packet, "%s",TmpLotId);
  }
  if (TLK_CltSubmit (&dmq, &packet) != _DMQ_SUCCESS)
  {
    strcpy ( ErrMsgC, TRC_GetTime() );
    strcat ( ErrMsgC,DMQ_ErrorMsg);
    *RetSts=1;
    return;
  }
  else if ( TLK_CltReceive ( &dmq, &packet ) != _DMQ_SUCCESS )
  {
    strcpy ( ErrMsgC, TRC_GetTime() );
    strcat ( ErrMsgC,DMQ_ErrorMsg);
    *RetSts=1;
    return;
  }
/* From Reply Get first field .i.e. The number of lots received */

  if (TLK_GetFirstField (&packet, &RecvLots ,
                         _TLK_INT) != _TLK_SUCCESS)
  {
    strcpy ( ErrMsgC,TRC_GetTime() );
    strcat ( ErrMsgC, "Error in Decoding Reply ");
    *RetSts=1;
    return ; 
  }

  if ( RecvLots != *num_lots )
  {
    strcpy ( ErrMsgC,TRC_GetTime() );
    strcat ( ErrMsgC, "Number of lots less than Expected lots ");
    *RetSts=1;
    return ; 
  }
  for ( TmpVar =0; TmpVar < *num_lots ; TmpVar ++)
  {

    if (TLK_GetNextField (&packet, Ptr,
                             sizeof (Ptr)) != _TLK_SUCCESS)
    {
      strcpy ( ErrMsgC,TRC_GetTime() );
      strcat ( ErrMsgC, "Error in getting Field from reply- Lot id ");
      *RetSts=1;
      return ; 
    }
    if (TLK_GetNextField (&packet, LotEqpId,
                             EQP_ID_LEN ) != _TLK_SUCCESS)
    {
      strcpy ( ErrMsgC,TRC_GetTime() );
      strcat ( ErrMsgC, "Error in getting Field from reply- equipment id ");
      *RetSts=1;
      return ; 
    }
    if (TLK_GetNextField (&packet, LotLocId,
                             LOC_ID_LEN ) != _TLK_SUCCESS)
    {
      strcpy ( ErrMsgC,TRC_GetTime() );
      strcat ( ErrMsgC, "Error in getting Field from reply- location id ");
      *RetSts=1;
      return ; 
    }
    EqpLen = strlen(LotEqpId);

    strncpy ( McsLocation[TmpVar],_ALL_SPACES,EQP_ID_LEN+LOC_ID_LEN+1 );
    strncpy ( McsLocation[TmpVar],LotEqpId,EqpLen);

    if ( ! all_space(LotEqpId) )
      McsLocation[TmpVar][EqpLen ]= ',';
    else
      McsLocation[TmpVar][EqpLen ]= ' ';

    strncpy(&McsLocation[TmpVar][EqpLen + 1 ],LotLocId,strlen(LotLocId) );
  
  }
  return ;
}

/****************************END OF FUNCTION get_mcs_info ************/
