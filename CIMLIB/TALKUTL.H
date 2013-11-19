/******************************************************************************/
/*                                                                            */
/*   talkutl.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements and prototypes for functions that   */
/*   communicate with the various transports (includes talkMBX.c, talkDMQ.c). */
/*                                                                            */
/******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _TLK_TALKUTL
#define _TLK_TALKUTL


#include "cimdefs.h"
#ifdef WINNT
#include <windows.h>
#define WINNTAPI FAR PASCAL
#else
#define WINNTAPI 
#endif
#ifdef WIN32
#define sleep(x) Sleep(x * 1000 )
#endif
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


#include <p_entry.h>
#include <p_proces.h>
#include <p_group.h>
#include <p_typecl.h>
#include <p_return.h>
#include <p_symbol.h>

#endif

/******************* End of DMQ specific include statements *******************/
/******************************************************************************/

/*****VERSION*********************************************************/

static char talkutlhid[] = "$Id: talkutl.h Ver 01.0 Rel 01.0 $";

/*********************************************************************/

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _TLK_TYPPTP       1                    /* Promis TP style message     */
#define _TLK_TYPVFEI      2                    /* VFEI style message          */
#define _TLK_TYPFAST      3                    /* FASTech style message       */
#define _TLK_TYPDEL       4                    /* Standard delimited message  */
#define _TLK_TYPWINBOND   5                    /* Winbond style message       */
#define _TLK_TYPFREEFRM   6                    /* Free format output          */

#define _TLK_TGTSPC_START             1        /* Handles for CIM transactions*/

#define _TLK_TGTBCX_SETUP_DC          2        /* ...  BCX HANDLES            */
#define _TLK_TGTBCX_TRACE_LIST        5
#define _TLK_TGTBCX_GET_EQPT_CATG     6
#define _TLK_TGTBCX_ADD_LOT           7
#define _TLK_TGTBCX_REM_LOT           8
#define _TLK_TGTBCX_CREATE_BATCH      9
#define _TLK_TGTBCX_UPDATE_STARTTIME 10
#define _TLK_TGTBCX_UPDATE_ENDTIME   11
#define _TLK_TGTBCX_UPDATE_BATCH     12
#define _TLK_TGTBCX_UPDATE_BATCH_STATE 13
#define _TLK_TGTBCX_GETEQPTINFO      14
#define _TLK_TGTBCX_DELETE_BATCH     15
#define _TLK_TGTBCX_GETBATCHLIST     16  
#define _TLK_TGTBCX_GETBATCHPARAM    17


#define _TLK_TGTPTP_HOLD_LOT        20        /* PROMIS HANDLES              */
#define _TLK_TGTPTP_ABORT_LOT        21       /* PROMIS HANDLES              */
#define _TLK_TGTPTP_TRACKIN          22
#define _TLK_TGTPTP_TRACKOUT         23
#define _TLK_TGTPTP_VALIDATELOT      24
#define _TLK_TGTPTP_VERIFYUSER       25
#define _TLK_TGTPTP_GETJOBFILE       26
#define _TLK_TGTPTP_GETMETRECP       27
#define _TLK_TGTPTP_CHECKLOT         28
#define _TLK_TGTPTP_OPERINST         29
#define _TLK_TGTPTP_FORMTAGCMD       30
#define _TLK_TGTPTP_GET_COMPID       31
#define _TLK_TGTPTP_GET_REWORK       32
#define _TLK_TGTPTP_UPDATELOT        33
#define _TLK_TGTPTP_GETEQPSTATE      34
#define _TLK_TGTPTP_SETEQPSTATE      35

//#define _TLK_TGTADC_ALARM_LIST       41       /* ADC HANDLES                  */
#define _TLK_TGTADC_RESET_BATCH      42       
#define _TLK_TGTADC_SETUP_BATCH      43      
#define _TLK_TGTADC_RESET_TRACE      44    
#define _TLK_TGTADC_SETUP_TRACE      45   
#define _TLK_TGTADC_SEND_REPORT1     46  
#define _TLK_TGTADC_SEND_REPORT2     47
#define _TLK_TGTADC_SETUPDC          48 
#define _TLK_TGTADC_DCCOMPLETE       49 



#define _TLK_TGTEST_LOGLOT           51       /* EST RELATED HANDLES         */
#define _TLK_TGTEST_ADDCMT           52
#define _TLK_TGTEST_GETINFO          53
#define _TLK_TGTEST_SETINFO          54

#define _TLK_TGTKBS_MOVEINTOLAYER    61       /* KBS RELATED HANDLES         */

/* Handles for Lot reseration related transactions */
#define _TLK_TGTLRS_RESLOT           66      
#define _TLK_TGTLRS_FREELOT          67       
#define _TLK_TGTLRS_GETRESSTATUS     68      

#define _TLK_TGTRECP_GETRECP         71       /* RECIPE SERVER HANDLE        */
#define _TLK_TGTRECP_GETSPSR         72     

#define _TLK_TGTRTS_SETLOCATION      76       /* RTS server handles          */
#define _TLK_TGTRTS_VALIDATE_RETICLE 77
#define _TLK_TGTRTS_TRACKIN          78
#define _TLK_TGTRTS_TRACKOUT         79
#define _TLK_TGTMCS_CLRLOCATION      80
#define _TLK_TGTMCS_SETLOCATION      81
#define _TLK_TGTPTP_GET_DCOPSTS      82
#define _TLK_TGTPTP_ENTER_DATA       83


#define _TLK_TGTALARM_ALARM_LIST     88       /* ALARM handles                  */
#define _TLK_TGTALARM_SEND_REPORT    89

#define _TLK_TGTPRI_CARRIERENT           91       /* PRI server handles     */
#define _TLK_TGTPRI_CARRIERREM           92       
#define _TLK_TGTPRI_LOCCHANGE            93       /* PRI server handles     */ 
#define _TLK_TGTPRI_SETLOTLOCATION           94      
#define _TLK_TGTPRI_RETRIEVELOT           95       
#define _TLK_TGTPRI_CLEARLOTLOCATION           96       


#define _TLK_TGTVFEI                 100
#define _TLK_TGTVFEI0                100
#define _TLK_TGTVFEI1                101
#define _TLK_TGTVFEI2                102

#define _TLK_TGT_EMG                 201
#define _TLK_TGT_EMR                 202
#define _TLK_TGT_EMG_GETEQPTRCP          203
#define _TLK_TGT_EMG_GETMETEQPTRCP       204


#define _TLK_DMQ_NORPLY   30                /* message class for no reply  */
#define _TLK_DMQ_NOQUEUE   -1                /* code for no queue handle    */
#define _TLK_DMQ_RETRY     5                 /* maximum retries on DMQ      */

#define _DMQ_TRC_CONNECT   "%s - Connected to queue <%s> at address <%d.%d>.\n"
#define _DMQ_TRC_CONTARG   "%s - Set target for host <%s> at address <%d.%d>.\n"
#define _DMQ_TRC_RELEASE   "%s - Releasing queue <%s> at address <%d.%d>.\n"
#define _DMQ_TRC_RELTARG   "%s - Releasing target <%s> at address <%d.%d>.\n"
#define _DMQ_TRC_SHUTDOWN  "%s - Shutting down DMQ transport.\n"

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


/* Server Queue Names */
#define _TLK_PROMISTP_QNAME     "PSC_TPSERVER"
#define _TLK_PROMISRPM_QNAME    "PSC_RPMSERVER"
#define _TLK_BCX_QNAME          "BCXSRV"
#define _TLK_ALM_QNAME          "ALMSRV"
#define _TLK_OCAP_QNAME         "OCAPSRV"
#define _TLK_NOTYFY_QNAME       "NOTFNSRV"      /* Notification server name   */  
#define _TLK_EMGUI_QNAME        "EMGUI"
#define _TLK_ADCSQL_QNAME       "ADCSQLSRV"
#define _TLK_SPCCTLSRV_QNAME    "SPCCTLSRV"
#define _TLK_PRISRV_QNAME           "PRISRV"
#define _TLK_ADC_QNAME          "ADCSRV"
#define _TLK_SPCSQL_QNAME       "SPCSQLSRV"

/**** Handles for Servers ****/

#define _TLK_TGT_PROMISTP   301


/********************** End of parameter definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of structure definitions *************************/


typedef struct _TLK_dmqTgt {         /* Structure for a DMQ host              */
  int target;                        /* handle representing the target        */
  char name[_SIZ_DMQNAME];           /* name of the host queue                */
  q_address queue;                   /* actual host queue address             */
  int timeout;                       /* timeout value to be used for replies  */
  int rplyHandle;                    /* handle for DMQ queue record           */
  char logFile[_SIZ_FILENAME];       /* Logfile name for input and output messages */
  int  verbose;                      /* Verbose level for messages */
} TLK_dmqTgt;

typedef struct _TLK_dmqRec {         /* Structure for a DMQ queue             */
  int handle;                        /* handle representing the queue         */
  char name[_SIZ_DMQNAME];           /* name of the queue                     */
  q_address queue;                   /* actual queue number                   */
  q_address filter;                  /* filter to use for exclusive read      */
  q_address rplyQueue;               /* queue requested as the reply queue    */
  int32 attachMode;                  /* attach mode for pams_attach_q         */
  int32 attachType;                  /* queue type for pams_attach_q          */
  struct psb psb;                    /* DECmessageQ status block              */
  short dmqclass;                    /* message class code                    */
  short dmqtype;                     /* message type code                     */
  char logFile[_SIZ_FILENAME];       /* Logfile name for input and output messages */
  int  verbose;                      /* Verbose level for messages */
} TLK_dmqRec;

typedef struct _TLK_mesg {           /* Structure for the message packet      */
  char buffer[_SIZ_MSGBUFF];         /* contents of the message               */
  char command[_SIZ_GENFIELD];       /* string with command generating reply  */
  char host[_SIZ_GENFIELD];          /* string with host name for command     */
  int  seqToken;                     /* token with sequence number (server)   */
  char delimit;                      /* delimitor used for delimited fields   */
  int  msgLen;                       /* number of bytes currently in message  */
  int  msgPtr;                       /* pointer into message for parsing      */
  int  msgSeq;                       /* message sequence number (client)      */
  int  msgType;                      /* integer representing the message type */
} TLK_mesg;

/********************** End of structure definitions **************************/
/******************************************************************************/


/******************************************************************************/
/*********************** Start of global definitions **************************/

TLK_mesg  message;

/************************ End of global definitions ***************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of function prototypes **************************/


int  TLK_DMQ_SetQueue (int  handle,
                      char *qName,
                      long int attMode,
                      char *logFile,
                      int  verbose);

int  TLK_DMQ_SetTarget (int  target,
                       char *hostName,
                           int rplyHandle,
                       int  timeout,
                       char *logFile,
                       int  verbose);
int TLK_DMQ_SetTargetQNumber (int  target,
                       int group,
                           int queue,
                           int  rplyHandle,
                       int  timeout,
                       char *logFile,
                       int  verbose);


int  TLK_DMQ_SetTimeout (int target,
                        int timeout);

int  TLK_DMQ_ClrQueue (int  handle);
                      

int  TLK_DMQ_ClrTarget (int  target);
                      

void  TLK_DMQ_Shutdown (void);

int  TLK_DMQ_ClientPut (int target);
                      

int  TLK_DMQ_ClientGet (int target);
                       

int  TLK_DMQ_ServerGet (int handle,
                       int msgType, char *msgstr);
                       
int  TLK_DMQ_ServerGetNoWait (int  handle,
                       int  msgType);
                       
int  TLK_DMQ_ServerPut (int  handle);
                      

void  TLK_DMQ_Flush (TLK_dmqRec *dmqRec);

int  TLK_DMQ_Confirm (TLK_dmqRec *dmqRec);

int  TLK_DMQ_SetError (char *mesg,
                      long status);

int  TLK_DMQ_LocQueue (int        *ident,
                      TLK_dmqRec *elem);

int  TLK_DMQ_CmpQueue (TLK_dmqRec *elem1,
                      TLK_dmqRec *elem2);

int  TLK_DMQ_LocQTgt (int        *ident,
                     TLK_dmqTgt *elem);

int  TLK_DMQ_CmpQTgt (TLK_dmqTgt *elem1,
                     TLK_dmqTgt *elem2);
int  TLK_DMQ_ServerGeta (       int  handle,
                                int  msgType,
                                void (*DmqDispatch)() ,TLK_dmqRec **Ptr );

void  TLK_LogInpMsg (char *logFile,

                        int  verbose);

void  TLK_LogOutMsg (char *logFile,

                        int  verbose);

void  TLK_Flush (TLK_dmqRec *dmqRec);

int  TLK_SetError (char *mesg,
                      long status);

int TLK_DMQ_GetQDetail(int target,
                            int  *GroupId,
                       int  *QueueId,char *logfile,int verbose);

int TLK_DMQ_GetQ2Detail(int target,
                        q_address *que_addr,
                        char *logfile,int verbose);

int TLK_DMQ_GetQ3Detail(int target,
                        q_address *que_addr,
                        char *logfile,int verbose);

int32 TLK_DMQ_PutSBSReg ( q_address q_addr,
                          q_address mot_q_addr );

int32 TLK_DMQ_GetSBSReply( );

int TLK_CmpQTgt (TLK_dmqTgt *elem1,
                     TLK_dmqTgt *elem2);

int  TLK_LocQTgt (int        *ident,
                     TLK_dmqTgt *elem);

int TLK_DMQ_TimeOut ();
int TLK_DMQ_ClearTimer (int32 *timer_id);
int TLK_DMQ_CreateTimer( int *TimerId, int TimeInSec );

/*********************** End of function prototypes ***************************/
/******************************************************************************/
#ifdef WINNT
int  WINNTAPI TLK_DMQ_VB_GetQDetail (int Target,int  *GroupId,
                       int  *QueueId,BSTR logFile,int  verbose);

int             WINNTAPI TLK_DMQ_VB_SetQueue (int  handle,BSTR qName,long int attMode,
                                                                        BSTR logFile,int  verbose);

int             WINNTAPI TLK_DMQ_VB_SetTarget (int  target, BSTR hostName,int rplyHandle,
                       int  timeout,BSTR logFile, int  verbose);
int  WINNTAPI TLK_DMQ_VB_SetTargetQNumber (int  target,int group,int queue,int  rplyHandle,
                       int  timeout,BSTR logFile,int  verbose);

int             WINNTAPI TLK_DMQ_VB_ClientPut (int target);
                      
int             WINNTAPI TLK_DMQ_VB_ClientGet (int target);
                       
void    WINNTAPI TLK_DMQ_VB_Shutdown (void);

int             WINNTAPI TLK_DMQ_VB_ServerGet (int handle, int msgType, BSTR *msgstr);
                       
int             WINNTAPI TLK_DMQ_VB_ServerGetNoWait (int  handle,int  msgType);
                       
int             WINNTAPI TLK_DMQ_VB_ServerPut (int  handle);

void    WINNTAPI TLK_VB_LogInpMsg (BSTR logFile,int  verbose);

void    WINNTAPI TLK_VB_LogOutMsg (BSTR logFile,int  verbose);
#endif

#endif

#ifdef __cplusplus
}
#endif
/******************************************************************************/
/*************************** End of file talkutl.h ****************************/
/******************************************************************************/
