/******************************************************************************/
/*                                                                            */
/*   parsutl.h:                                                               */
/*   ~~~~~~~~~~                                                               */
/*                                                                            */
/*   This file contains define statements and prototypes for functions that   */
/*   parse messages (includes functions in parsPTP.c and parsFST.c).          */
/*                                                                            */
/******************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif

#ifndef _PRS_PARSUTL
#define _PRS_PARSUTL

#ifdef WINNT
#include <Windows.h>
#define WINNTAPI FAR PASCAL 
#else
#define WINNTAPI 
#endif

/*****VERSION*********************************************************/

static char parsutlhid[] = "$Id: parsutl.h Ver 01.0 Rel 01.0 $";

/*********************************************************************/

/******************************************************************************/
/********************* Start of parameter definitions *************************/

#define _PRS_CHAR         -1                   /* refers to char type         */
#define _PRS_INT          -2                   /* refers to int type          */
#define _PRS_DOUBLE       -3                   /* refers to double type       */
#define _PRS_FLOAT        -4                   /* refers to float type        */
#define _PRS_TIME         -5                   /* refers to time type         */
#define _PRS_UINT         -6                   /* refers to unsigned int 4     */
#define _PRS_BIN          -7                   /* refers to binary            */
#define _PRS_UINT1                -8                                    /* refers to unsigned int 1   */
#define _PRS_UINT2                -9                                    /* refers to unsigned int 1   */

#define _PRS_MANDATORY    1                    /* mandatory token             */
#define _PRS_OPTIONAL     2                    /* optional token              */

#define _PRS_STRFLD       "%s|"                /* proto to parse WIN string   */
#define _PRS_INTFLD       "%d|"                /* proto to parse WIN int      */

#define _PRS_INPCMD        "%s - Received command from <%s> (%d bytes):\n"
#define _PRS_OUTREP        "%s - Sending reply to <%s> (%d bytes):\n"

#define _PRS_PTP_DELIMIT  '|'                  /* standard delimiter to use   */
#define _PRS_PTP_STASTRT  8                    /* TP message status position  */
#define _PRS_PTP_STALEN   40                   /* length of TP status string  */
#define _PRS_PTP_MSGSTRT  49                   /* TP message start position   */
#define _PRS_PTP_CMDSTRT  49                   /* TP query start position     */

#define _PRS_PTP_SUCMSG  "SUCCESS                                 "
#define _PRS_PTP_CMDSTR  "1C      %-40.40s%c"

#define _PRS_PTP_ABORT    "LOTSPEC_ABORTSTEP"
#define _PRS_PTP_TRACKOUT "LOTTRACK_OUTOFSTEP"
#define _PRS_PTP_LOTINFO  "LOTQUERY_LOTINFO"
#define _PRS_PTP_TBLINFO  "LOTQUERY_TBLINFO"

#define _PRS_MCS_SUCMSG  "MCS_SUCCESS"

#define _PRS_VFI_MAXTID  999999999
#define _PRS_VFI_DELIMIT ' '
#define _PRS_VFI_CMDID  "CMD"
#define _PRS_VFI_MID    "MID"
#define _PRS_VFI_TID    "TID"
#define _PRS_VFI_MTY    "MTY"
#define _PRS_VFI_EVENTID "EVENT_ID"
#define _EVENT_REPORT    "EVENT_REPORT"
#define _PRS_VFI_MACHCMD "MACH_CMD"
#define _PRS_VFI_CMDTYP  "CMD_TYPE"


#define _PRS_WIN_MAXTID  999999999
#define _PRS_WIN_CMDSTR  "%s TID=%d"

/********************** End of parameter definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************* Start of structure definitions *************************/

typedef struct _PRS_cmdTok {         /* Structure to reference tokens in msg  */
  char *label;                       /* pointer to label in message           */
  char *value;                       /* pointer to value in message           */
  int labelLen;                      /* number of characters in label         */
  int valueLen;                      /* number of characters in value         */
} PRS_cmdTok;

/********************** End of structure definitions **************************/
/******************************************************************************/


/******************************************************************************/
/********************** Start of function prototypes **************************/

int  PRS_PTP_InitCommand (char *command);


int  PRS_PTP_ChkStatus (void);

char * PRS_PTP_GetStatus (void);

int  PRS_PTP_GetFirst (void *data,
                      int  type);

int  PRS_PTP_GetNext (void *data,
                     int  type);

#ifdef WINNT
int  PRS_PTP_PutToken (char *format,...);
#else
int   PRS_PTP_PutToken ();
#endif

void PRS_PTP_LogOutMsg (char *logFile,
                        int  verbose);

void  PRS_PTP_LogInpMsg (char *logFile,
                        int  verbose);
void  PRS_PTP_GetSubField (char *InStr,char Del,char *Value);

/****************************************************************************/
/* Parse VFEI functions                                                     */

int  PRS_VFI_InitCommand (char *command, char *machine);

int  PRS_VFI_InitEvent (char *Event, char *machine);

int  PRS_VFI_InitReply (char *Event, char *machine);
 
int  PRS_VFI_PutToken (char *vid, int  type, void *data);

int  PRS_VFI_ChkStatus (void);

int  PRS_VFI_ChkMsgSeq (void);

int  PRS_VFI_GetToken (char *vid, int  type, void *data);

int  PRS_VFI_ChkInpMsg (void);

int  PRS_VFI_GetArray (char *vid, int  maxElem, int  type,
                      int  *numElem, void *data);

int  PRS_VFI_PutArray (char *vid, int  type, int  numElem, void *data);

char *PRS_VFI_SetPattern (char *vid, int  type);

int  PRS_VFI_SetToken (char *source, int  type, void *data);

int  PRS_VFI_GetNext (char *data, int  size);

void PRS_VFI_LogOutMsg (char *logFile, int  verbose);

void PRS_VFI_LogInpMsg (char *logFile, int  verbose);


/*********************** End of function prototypes ***************************/
/******************************************************************************/


/******************************************************************************/
/*************************** End of file parsutl.h ****************************/
/******************************************************************************/

#ifdef WINNT
int WINNTAPI PRS_VFI_VB_InitCommand (BSTR command, BSTR machine);

int WINNTAPI PRS_VFI_VB_InitEvent (BSTR Event, BSTR machine);

int WINNTAPI PRS_VFI_VB_InitReply (BSTR Event, BSTR machine);
 
int WINNTAPI PRS_VFI_VB_PutTokenStr (BSTR vid, int  type, BSTR data);

int WINNTAPI PRS_VFI_VB_PutTokenNum( BSTR vid,int type, int Value);

int WINNTAPI PRS_VFI_VB_PutTokenFloat( BSTR vid,int type, float Value);

int WINNTAPI PRS_VFI_VB_PutArrayStr (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY *array);
int WINNTAPI  PRS_PTP_VB_GetMsgBuffer (BSTR *msg) ;

int WINNTAPI PRS_VFI_VB_PutArrayNum (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY  *array);

int WINNTAPI PRS_VFI_VB_GetTokenStr (BSTR vid, int  type, BSTR *data);

int WINNTAPI PRS_VFI_VB_GetTokenNum (BSTR vid, int  type, int  *data); 

int WINNTAPI PRS_VFI_VB_GetTokenFloat (BSTR vid, int  type, float *data); 

int WINNTAPI PRS_VFI_VB_GetArrayStr (BSTR vid,int maxElem,int type,int *numelem,
                                     LPSAFEARRAY *array);

int WINNTAPI PRS_VFI_VB_GetArrayNum (BSTR vid,int maxElem,int type,int *numelem,
                                     LPSAFEARRAY *array);
int WINNTAPI PRS_VFI_VB_GetArrayFloat (BSTR vid, int maxelem,
                                                                         int type, int*numelem,
                                                                         LPSAFEARRAY *array);

int WINNTAPI PRS_VFI_VB_PutArrayFloat (BSTR vid,int type,int numelem,
                                     LPSAFEARRAY *array);

/* PRomis transport functions */

int  WINNTAPI PRS_PTP_VB_InitCommand (BSTR command);

int  WINNTAPI PRS_PTP_VB_ChkStatus (void);

int  WINNTAPI PRS_PTP_VB_GetFirstStr (BSTR *data,int  type);

int  WINNTAPI PRS_PTP_VB_GetFirstNum (int *data,int  type);

int  WINNTAPI PRS_PTP_VB_GetNextStr (BSTR *data,int  type);

int  WINNTAPI PRS_PTP_VB_GetNextNum (int *data,int  type);

int  WINNTAPI PRS_PTP_VB_PutTokenStr (BSTR msg);

int  WINNTAPI PRS_PTP_VB_PutTokenNum (int msg);

void  WINNTAPI PRS_PTP_VB_GetStatus (BSTR *Value);
void  WINNTAPI PRS_PTP_VB_GetSubField (BSTR InStr,char Del,BSTR *Value);
void  WINNTAPI PRS_PTP_VB_SetDelimiter (char Value);


#endif

int  PRS_DEL_InitCommand (char *command,
                         char *comment);
int  PRS_DEL_PutToken ();
int  PRS_DEL_ChkStatus (char *success);
int  PRS_DEL_SetDelim (char delimit);
int  PRS_DEL_GetNext (void *data,
                     int  type);
void  PRS_DEL_LogOutMsg (char *logFile,
                        int  verbose);
void  PRS_DEL_LogInpMsg (char *logFile,
                        int  verbose);
void PRS_PTP_SetDelimiter (char Value);

#endif
#ifdef __cplusplus
}
#endif
