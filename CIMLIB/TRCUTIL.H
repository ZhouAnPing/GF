/******************************************************************************/
/*                                                                            */
/*   trcutil.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements and prototypes used by the trace    */
/*   utility functions in trcutil.c.                                          */
/*                                                                            */
/******************************************************************************/

/*****VERSION*********************************************************/


/*********************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _TRC_TRCUTIL
#define _TRC_TRCUTIL
#ifdef WIN32
    #include <windows.h>
    #define WINNTAPI FAR PASCAL
#else
    #define WINNTAPI 
#endif
static char trcutilhid[] = "$Id: trcutil.h Ver 01.0 Rel 01.0 $";
#include "cimdefs.h"

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _TRC_INITSTR       "%s: Trace file <%s> set to verbose level %d.\n"
#define _TRC_SHUTSTR       "%s: Trace file <%s> has been closed.\n"
#define _TRC_CMDSTR        "%s: Received command <%s>.\n"
#define _TRC_SPSTR         "%s: Received command <set %s=%s>.\n"
#define _TRC_SPINT         "%s: Received command <set %s=%d>.\n"
#define _TRC_OUTSTR        "%s: Sending %d bytes to %s:\n"
#define _TRC_INPSTR        "%s: Received %d bytes from %s:\n"
#define _TRC_INPCMD        "%s: Received command from <%s> (%d bytes):\n"
#define _TRC_OUTREP        "%s: Sending reply to <%s> (%d bytes):\n"
#define _TRC_NEWLIN        "-----------------\n"
#define _TRC_DATA          -1

#define _TRC_FATAL         'F'
#define _TRC_WARN          'W'
#define _TRC_INFO          'I'

#define _TRC_SUCCESS       "Success"
#define _TRC_SHUTDOWN      "Application is shutting down"
#define _TRC_GENERIC       "%s"
#define _TRC_BADFILENAME   "Filename <%s> is invalild"
#define _TRC_FILEOPENERR   "Could not open file <%s>"
#define _TRC_FILEWRITERR   "Could not write to file <%s>"
#define _TRC_NOMEMORY      "Ran out of memory"
#define _TRC_FLDMISSING    "Field <%s> is missing"
#define _TRC_FLDNOTXPCT    "Field <%s> is unexpected"
#define _TRC_FLDTOOLONG    "Field <%s> should be less that %d characters"
#define _TRC_FLDBADINTV    "Field <%s> can't be set to %d"
#define _TRC_FLDBADSTRV    "Field <%s> can't be set to <%s>"
#define _TRC_BADMBOXNAME   "Mailbox name <%s> is invalild"
#define _TRC_MBOXNOTOPN    "%s mailbox has not been opened"
#define _TRC_MBOXNOTSND    "Could not send to mailbox <%s>"
#define _TRC_MBOXNOTRCV    "Could not receive from mailbox <%s>"
#define _TRC_TIMEOUT       "Timeout while waiting for reply from <%s>"
#define _TRC_HOSTERROR     "%s"
#define _TRC_PARSEERR      "Error parsing <%s>"
#define _TRC_PARSEEND      "Reached end of parse buffer"
#define _TRC_PARSESEQ      "Received message #%d when expecting #%d"
#define _TRC_RECNOTFND     "Record not found for <%s>"
#define _TRC_RECEXISTS     "Record already exists for <%s>"
#define _TRC_DMQERROR      "%s"
#define _TRC_CMDWRONG      "Received command <%s> when expecting <%s>"
#define _TRC_CMDINVALID    "Received invalid command <%s>"
#define _TRC_FAILURE       "Undefined error"
#define _TRC_NOARMID       "Arm Id missing from incoming message"
#define _TRC_BADARMID      "Wrong Arm Id = <%d> received from incoming message"
#define _TRC_LOAD_FAILURE  "Loading failed on Arm Id = <%d> "
#define _TRC_TAGREADFAIL   "Read Tag failure on Arm = <%d> "
#define _TRC_TAGWRITEFAIL  "Write Tag failure on Arm = <%d> "
/********************** End of parameter definitions **************************/
/******************************************************************************/
#define _TRC_LVL_DEBUG   8        /* Debug/trace messages */
#define _TRC_LVL_INFO    8        /* Informational messages */
#define _TRC_LVL_WARN    4   /* Non-fatal errors, can be handled within the program,
                                don't result in program termination */
#define _TRC_LVL_ERROR   1   /* Fatal errors, resulting in program termination */
#define _TRC_LVL_STATE   1   /* Status changes -e.g. server startup/shutdown/queue shutdown etc */
#define _TRC_NEXT_DAY    86400    /* number of seconds in one day */
#define _LOG_2MB_LIMIT     2097152  /* 2MB limit to filesize */
/******************************************************************************/
/*********************** Start of extern definitions **************************/

extern char alrMesg[_SIZ_MSGBUFF];
extern int alrCode;

/************************ End of extern definitions ***************************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of structure definitions *************************/

typedef struct _TRC_alrdef {         /* Structure for an alarm definition     */
  int code;                          /* alarm code number                     */
  int disable;                       /* flag to indicate if alarm is disabled */
  char severity;                     /* typically I, W or F                   */
  char message[_SIZ_GENFIELD];       /* message prototype of alarm            */
} TRC_alrdef;

typedef struct _TRC_handle {         /* Structure for a file specification    */
  char alias[_SIZ_FILENAME];         /* link to true stream if aliased        */
  char name[_SIZ_FILENAME];          /* name of file related to identifier    */
  int  verbose;                      /* verbose level selected for file       */
  int  hasTraced;                    /* flag to say that trace has occured    */
  time_t nexOpenTime;                /* Time when next file should be opened  */
  int    chgTime;                    /* duration between opening a new file   */
  long int MaxSize;                  /* Max filesize before creating newfile  */
} TRC_handle;

/********************** End of structure definitions **************************/
/******************************************************************************/


/******************************************************************************/
/************************ Start of macro definitions **************************/

#define TRC_CmdSetInt(alias,name,value) \
  TRC_Send (alias, _TRC_LVL_STATE, _TRC_SPINT, TRC_GetTime(), name, value)

#define TRC_CmdSetStr(alias,name,value) \
  TRC_Send (alias, _TRC_LVL_STATE, _TRC_SPSTR, TRC_GetTime(), name, value)


/************************ End of macro definitions ****************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of function prototypes **************************/

int     TRC_InitAlarm (char *alias,char *appName,void (*shutdown)());
int     TRC_AddAlarm (int  alarmCode,char alarmSevr,char *alarmMesg);
void    TRC_ClrAlarm (void);
void    TRC_EnableAlarm (int alarmCode);
void    TRC_DisableAlarm (int alarmCode);
int     TRC_Clean (char *alias);
/*...Opens new trace file every _TRC_NEXT_DAY (86400) seconds...*/
int     TRC_Open (char *alias, char *name, int  verbose); 
/*...Opens new trace file every Duration seconds...*/
int     TRC_OpenEx (char *alias, char *name, int  verbose, int Duration, long Fsize);
int     TRC_Close (char *alias);
void    TRC_Shutdown (void);

#ifdef WIN32
   int      TRC_SetAlarm (char *funcName,int errCode,... );
   int      TRC_Send (char *alias, int level,char *format,...);
   int      TRC_QSnd (TRC_handle *trcRec, char *format,...);
#else
   int      TRC_SetAlarm ();
   int      TRC_Send ();
   int      TRC_QSnd ();
#endif

int      TRC_SetVerb (char *alias, int  verbose);
int      TRC_GetVerb (char *alias);
int      TRC_NewLine (char *alias);
char    *TRC_GetTime (void);
int      TRC_LocFile (char *ident, TRC_handle *elem);
int      TRC_CmpFile (TRC_handle *elem1, TRC_handle *elem2);
int      TRC_LocAlrm (int  *ident, TRC_alrdef *elem);
int      TRC_CmpAlrm (TRC_alrdef *elem1, TRC_alrdef *elem2);
char    *TRC_ErrorMsg();
int      TRC_ChkAlarm ();
int      TRC_CmpAlarm(int code);
int      TRC_GetAlarm (void);
int      TRC_NewTrcFilename(TRC_handle *trcRec);


/********************************************************************************/
/*         VB RELATED ROUTINES                                                   */
/********************************************************************************/

#ifdef WIN32
   int      WINNTAPI TRC_VB_InitAlarm (BSTR alias, BSTR appName);
   void     WINNTAPI TRC_VB_ClrAlarm (void);
   int      WINNTAPI TRC_VB_Open (BSTR alias,BSTR name,int  verbose);
   int      WINNTAPI TRC_VB_Close (BSTR alias);
   void     WINNTAPI TRC_VB_Shutdown(void );
   int      WINNTAPI TRC_VB_SetAlarm (BSTR funcName, int errCode, BSTR Value);
   int      WINNTAPI TRC_VB_Send (BSTR alias, int level,BSTR msg);
   void     WINNTAPI TRC_VB_GetTime ( BSTR *timeStr);
   void     WINNTAPI TRC_VB_GetErrorMsg ( BSTR *ErrMsg);
   int      WINNTAPI TRC_VB_ChkAlarm ();
#endif


#endif
#ifdef __cplusplus
}
#endif
/*********************** End of function prototypes ***************************/
/******************************************************************************/

/******************************************************************************/
/**************************** End of file trcutil.h ***************************/
/******************************************************************************/
