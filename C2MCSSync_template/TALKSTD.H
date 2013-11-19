/******************************************************************************/
/*                                                                            */
/*   talkstd.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements and prototypes for functions that   */
/*   manipulate the standard message structure (contained in talkstd.c).      */
/*                                                                            */
/******************************************************************************/

#ifndef _TLK_TALKSTD
#define _TLK_TALKSTD

#include "dmqutil.h"

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _TLK_SUCCESS      0                    /* successful completion       */
#define _TLK_FAILURE      99                   /* generic failure             */

#define _TLK_CLASS        222                  /* constant DMQ class          */
#define _TLK_FLDSZ        256                  /* maximum single field size   */

#define _TLK_CHAR         -1                   /* refers to char type         */
#define _TLK_INT          -2                   /* refers to int type          */
#define _TLK_DOUBLE       -3                   /* refers to double type       */
#define _TLK_FLOAT        -4                   /* refers to float type        */
#define _TLK_PID          -5                   /* refers to the PID type      */
#define _TLK_VAXTIME      -6                   /* refers to VAX time type     */

#define _TLK_MSG_GOTDATA  " %s - Received %d bytes from queue %d.%d\n"
#define _TLK_MSG_PUTDATA  " %s - Sent %d bytes to queue %d.%d\n"
#define _TLK_MSG_HEADER   " %s:  %s\n"
#define _TLK_MSG_DATAFLD  " -> %s\n"

/********************** End of parameter definitions **************************/
/******************************************************************************/

/******************************************************************************/
/********************* Start of structure definitions *************************/

/********************** End of structure definitions **************************/
/******************************************************************************/

/******************************************************************************/
/********************** Start of function prototypes **************************/

int TLK_SrvReceive(DMQ_parms *dmq, DMQ_pckt *packet);

int TLK_SrvSubmit(DMQ_parms *dmq, DMQ_pckt *packet, int xCode);

int TLK_CltReceive(DMQ_parms *dmq, DMQ_pckt *packet);

int TLK_CltSubmit(DMQ_parms *dmq, DMQ_pckt *packet);

char *TLK_GetStatusFld(DMQ_pckt *packet);

char *TLK_GetDescrFld(DMQ_pckt *packet);

int TLK_GetFirstField(DMQ_pckt *packet, void *data, int type);

int TLK_GetNextField(DMQ_pckt *packet, void *data, int type);

int TLK_PutMsgHeader(DMQ_pckt *packet, char *statStr, char *statMsg,
		char delimit);

int TLK_PutNextField();

/*********************** End of function prototypes ***************************/
/******************************************************************************/

#endif

/******************************************************************************/
/**************************** End of file talkstd.h ***************************/
/******************************************************************************/

