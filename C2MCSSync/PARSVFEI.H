/*
 * MCSSYNC.H
 *
 *  Created on: Jul 18, 2013
 *      Author: sid105131
 */

#ifndef MCSSYNC_H_
#define MCSSYNC_H_

int HSMS_VFI_GetToken(char *vid, int type, void *data);
int HSMS_VFI_GetArray(char *vid, int maxElem, int type, int *numElem, void *data);
char *HSMS_VFI_SetPattern(char *vid, int type);
int HSMS_VFI_SetToken(char *source, int type, void *data);
int HSMS_VFI_GetNext(char *data, int size);

#define _PRS_VFI_CMDID  "CMD"
#define _PRS_VFI_MID    "MID"
#define _PRS_VFI_TID    "TID"
#define _PRS_VFI_MTY    "MTY"
#define _PRS_VFI_EVENTID "EVENT_ID"
#define _EVENT_REPORT    "EVENT_REPORT"
#define _PRS_VFI_MACHCMD "MACH_CMD"
#define _PRS_VFI_CMDTYP  "CMD_TYPE"

#define _TOK_AMHSEQUIPID   "AMHSEQUIPID"
#define _TOK_CARRIERID   "CARRIERID"
#define _TOK_CATEGORY     "CATEGORY"
#define _TOK_NUMBER                     "NUMBER"

#define _SIZ_MSGBUFF       8000               /* maximum message buffer size */
#define _SIZ_GENFIELD      256
#define _SIZ_DUMMYNUMBER                20
#define _SIZ_LOTID         13                  /* lot id maximum size         */
#define _SIZ_AMHSEQUIPID                 9
#define _SIZ_CATEGORY                    9
#define _SIZ_NUMBER                      5

#define _RET_SUCCESS       0                   /* successful execution*/
#define _RET_PARSEERR      19                  /* parsing error  */
#define _RET_RECNOTFND     22                  /* record not found            */

#define _PRS_CHAR         -1                   /* refers to char type         */
#define _PRS_INT          -2                   /* refers to int type          */
#define _PRS_DOUBLE       -3                   /* refers to double type       */
#define _PRS_FLOAT        -4                   /* refers to float type        */
#define _PRS_TIME         -5                   /* refers to time type         */
#define _PRS_UINT         -6                   /* refers to unsigned int 4     */
#define _PRS_BIN          -7                   /* refers to binary            */
#define _PRS_UINT1                -8                                    /* refers to unsigned int 1   */
#define _PRS_UINT2                -9                                    /* refers to unsigned int 1   */

typedef struct _TLK_mesg { /* Structure for the message packet      */
  //char buffer[_SIZ_MSGBUFF]; /* contents of the message               */
  char *buffer;
  int msgLen; /* number of bytes currently in message  */
  int msgPtr; /* pointer into message for parsing      */
} TLK_mesg;

TLK_mesg message;

#endif /* MCSSYNC_H_ */
