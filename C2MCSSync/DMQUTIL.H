/******************************************************************************/
/*                                                                            */
/*   dmqutil.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains the parameter definitions and structure definitions   */
/*   for the library of DMQ functions.  It also contains the prototypes for   */
/*   functions defined in DMQutil.c.                                          */
/*                                                                            */
/******************************************************************************/

#ifndef _DMQ_DMQUTIL
#define _DMQ_DMQUTIL

/******************************************************************************/
/****************** Start of DMQ specific include statements ******************/

#ifdef VMS

#include <ssdef>
#include <stsdef>
#include <descrip>
#include <lib$routines>
#include <signal>

#include pams_c_entry_point
#include pams_c_process
#include pams_c_group
#include pams_c_type_class
#include pams_c_return_status_def
#include pams_c_symbol_def

#else

#ifndef u_short
typedef unsigned short u_short;
#endif

#include <p_entry.h>
#include <p_proces.h>
#include <p_group.h>
#include <p_typecl.h>
#include <p_return.h>
#include <p_symbol.h>

#endif

/******************* End of DMQ specific include statements *******************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _DMQ_FLDSZ      256                    /* maximum size of one field   */
#define _DMQ_BUFFSZ     30000                  /* DMQ message size            */
#define _DMQ_QNAMSZ     32                     /* DMQ queue name length       */
#define _DMQ_TIMEOUT    600                    /* timeout for client gets     */
                                      /* 1/10 of a second. 10=1 second */

#define _DMQ_RETRY      5                      /* maximum retries on DMQ      */

#define _DMQ_SERVER     1                      /* indicates server type queue */
#define _DMQ_CLIENT     2                      /* indicates client type queue */

#define _DMQ_TYP_REPLY  1000                   /* message type: one reply     */
#define _DMQ_TYP_QUIET  1001                   /* message type: no reply      */
#define _DMQ_TYP_RECOV  1002                   /* message type: recoverable   */

#define _DMQ_SUCCESS    0                      /* successful execution        */
#define _DMQ_NOATTACH   1                      /* could not attach to queue   */
#define _DMQ_NODMQGET   2                      /* could not receive message   */
#define _DMQ_NODMQPUT   3                      /* could not submit message    */
#define _DMQ_NODMQCNF   4                      /* could not confirm receipt   */
#define _DMQ_NOSERVER   5                      /* could not locate server     */
#define _DMQ_WRONGQ     6                      /* wrong queue for operation   */
#define _DMQ_TOUTERR    7                      /* timeout on DMQ operation    */

#define _PAMS_STR_DECLARED     "PAMS__DECLARED"
#define _PAMS_STR_NOOBJECT     "PAMS__NOOBJECT"
#define _PAMS_STR_RESRCFAIL    "PAMS__RESRCFAIL"
#define _PAMS_STR_NOTACTIVE    "PAMS__NOTACTIVE"
#define _PAMS_STR_PAMSDOWN     "PAMS__PAMSDOWN"
#define _PAMS_STR_TIMEOUT      "PAMS__TIMEOUT"
#define _PAMS_STR_NOTDCL       "PAMS__NOTDCL"
#define _PAMS_STR_EXCEEDQUOTA  "PAMS__EXCEEDQUOTA"
#define _PAMS_STR_EXHAUSTBLKS  "PAMS__EXHAUSTBLKS"
#define _PAMS_STR_REMQUEFAIL   "PAMS__REMQUEFAIL"
#define _PAMS_STR_NETNOLINK    "PAMS__NETNOLINK"
#define _PAMS_STR_NETLINKLOST  "PAMS__NETLINKLOST"
#define _PAMS_STR_AREATOSMALL  "PAMS__AREATOSMALL"
#define _PAMS_STR_INSQUEFAIL   "PAMS__INSQUEFAIL"
#define _PAMS_STR_NOTSUPPORTED "PAMS__NOTSUPPORTED"
#define _PAMS_STR_BADPARAM     "PAMS__BADPARAM"
#define _PAMS_STR_INVALIDNUM   "PAMS__INVALIDNUM"
#define _PAMS_STR_INVFORMAT    "PAMS__INVFORMAT"
#define _PAMS_STR_BADSELIDX    "PAMS__BADSELIDX"
#define _PAMS_STR_IDXTBLFULL   "PAMS__IDXTBLFULL"
#define _PAMS_STR_NO_DQF       "PAMS__NO_DQF"

#define _PAMS_MSG_DECLARED     "Queue is in use by another process"
#define _PAMS_MSG_NOOBJECT     "Queue is unknown"
#define _PAMS_MSG_RESRCFAIL    "Failed to allocate resource"
#define _PAMS_MSG_NOTACTIVE    "Queue is not currently active"
#define _PAMS_MSG_PAMSDOWN     "DMQ group is down"
#define _PAMS_MSG_TIMEOUT      "Function timed out"
#define _PAMS_MSG_NOTDCL       "Process is not declared to DMQ"
#define _PAMS_MSG_EXCEEDQUOTA  "Target process quota exceeded"
#define _PAMS_MSG_EXHAUSTBLKS  "No more message blocks available"
#define _PAMS_MSG_REMQUEFAIL   "Failed to properly dequeue message"
#define _PAMS_MSG_NETNOLINK    "No link or link lost to target group"
#define _PAMS_MSG_NETLINKLOST  "No link or link lost to target group"
#define _PAMS_MSG_AREATOSMALL  "Message received too large for buffer"
#define _PAMS_MSG_INSQUEFAIL   "Failed to properly queue a message"
#define _PAMS_MSG_NOTSUPPORTED "Operation not supported"
#define _PAMS_MSG_DEFAULT      "Refer to DMQ manual"
#define _PAMS_MSG_BADPARAM     "Bad argument value"
#define _PAMS_MSG_INVALIDNUM   "Invalid timer number passed to pams_set_timer"
#define _PAMS_MSG_INVFORMAT    "Invalid time format specified in call"
#define _PAMS_MSG_BADSELIDX    "Invalid or undefined selective receive index"
#define _PAMS_MSG_IDXTBLFULL   "Selective receive index table full"
#define _PAMS_MSG_NO_DQF       "No DQF for target queue"

/********************** End of parameter definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of structure definitions *************************/

typedef struct _DMQ_pckt {           /* Structure for DMQ message packet      */
  char      buffer[_DMQ_BUFFSZ];     /* message buffer                        */
  short int buffLen;                /* length of message buffer              */
  short int buffPtr;                 /* pointer into message buffer           */
  short int class;                  /* message class code                    */
  short int type;                    /* message type code                     */
} DMQ_pckt; 

typedef struct _DMQ_puts {           /* Structure for DMQ outbound messages   */
  q_address  target;                 /* target queue address                  */
  char       priority;               /* message priority                      */
  char       delivery,               /* delivery mode                         */
             uma;                    /* undeliverable message action          */
  q_address  respQue;                /* response queue                        */
  int32   timeout;                /* time to wait for delivery        pang     */
  struct psb psb;                    /* DECmessageQ status block              */
} DMQ_puts;

typedef struct _DMQ_gets {           /* Structure for DMQ inbound messages    */
  q_address  source;                 /* source queue address                  */
  char       priority;               /* message priority                      */
  int32   select;                 /* message selection mask      pang          */
  int32   timeout;                /* time to wait for delivery     pang        */
  struct psb psb;                    /* DECmessageQ status block              */
} DMQ_gets;

typedef struct _DMQ_parms {          /* Structure required for DMQ calls      */
  int       qType;                   /* type of queue (client / server)       */
  short int mClass;                  /* class of initiating query             */
  short int mType;                   /* type of initiating query              */
  int32  attachMode,              /* attach mode for pams_attach_q   pang      */
            attachType;              /* queue type for pams_attach_q          */
  char      name[_DMQ_QNAMSZ];       /* queue name for pams_attach_q          */
  long int  nameLen;                 /* length of queue name                  */
  q_address queue;                   /* actual queue number                   */
  DMQ_gets  get;                     /* required elements for inbound mesgs.  */
  DMQ_puts  put;                     /* required elements for outbound mesgs. */
} DMQ_parms;

/********************** End of structure definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of function prototypes **************************/

extern char DMQ_ErrorMsg[_DMQ_FLDSZ];

int DMQ_Shutdown (void);

int DMQ_ServerSet (DMQ_parms *dmq,
		   char      *name,
		   long int  attMode);

int DMQ_ServerGet (DMQ_parms *dmq,
		   DMQ_pckt  *packet);

int DMQ_ServerPut (DMQ_parms *dmq,
		   DMQ_pckt  *packet,
		   int       xCode);

int DMQ_ClientSet (DMQ_parms *dmq,
		   char      *name,
		   long int  attMode,
		   int *);

int DMQ_ClientSrv (DMQ_parms *dmq,
		   char      *name);

int DMQ_ClientGet (DMQ_parms *dmq,
		   DMQ_pckt  *packet);

int DMQ_ClientPut (DMQ_parms *dmq,
		   DMQ_pckt  *packet);

int DMQ_QueueSet (DMQ_parms *dmq,
		  char      *name,
		  long int  attMode);

int DMQ_AttachQ (DMQ_parms *dmq);

int DMQ_Receive (DMQ_parms *dmq,
		 DMQ_pckt  *packet);

int DMQ_Submit (DMQ_parms *dmq,
		DMQ_pckt  *packet);

int DMQ_Confirm (DMQ_parms *dmq);

void DMQ_Error (char *mesg,
		long status);

/*********************** End of function prototypes ***************************/
/******************************************************************************/

#endif

/******************************************************************************/
/************************** End of file dmqutil.h *****************************/
/******************************************************************************/






