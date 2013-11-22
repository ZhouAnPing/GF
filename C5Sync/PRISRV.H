#ifndef _INCLUDE_FORPRISRV_H
#define _INCLUDE_FORPRISRV_H

/*...Standard include files...*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*...CIMLIB include files...*/
#ifdef WIN32
    #include "cimdefs.h"
    #include "ordarray.h"
    #include "trcutil.h"
    #include "cimtokens.h"
    #include "parsutl.h"
    #include "talkutl.h"
#else
    #include "ssp_cimlib:cimdefs.h"
    #include "ssp_cimlib:ordarray.h"
    #include "ssp_cimlib:trcutil.h"
    #include "ssp_cimlib:cimtokens.h"
    #include "ssp_cimlib:parsutl.h"
    #include "ssp_cimlib:talkutl.h"
#endif

/*...MHServ include files...*/
#include "prisrv.h"
 

/*************************************************************************/
/**************** Start of parameter definitions *************************/
 
// #define LOG_NAME                "stderr"        /* Trace file name */
#define LOG_NAME                "C5Sync.log"       /* To resize logfile - WUL */
#define VERBOSITY               9               /* Trace file verbose level */
#define LOG_ALIAS               "C5SYNC_LOG"
#define C5MCSSYNCSRV_TIMEOUT          30              /* Default timeout period */
#define F3PRISRV_TIMEOUT        30              /* Default timeout period */
#define PRISRV_WAIT_FOREVER     0               /* Infinite wait period */
#define PRISRV_PAGELENGTH       50              /* Page length for DBLIST */

#define C5SYNCSRV_NAME         "C5SYNC"    /* Machine Id for C5SYNC */
#define TRANSNET_NAME          "MCSSIM"  //TODO change to "TRANSNET"     /* Machine Id for Transnet */
#define MCSINT_NAME            "MCSINT"       /* Machine Id for MCSINT */

#define GATEWAY_NAME            "GATEWAY"
#define EMG_NAME                "1ALIGN_EMG_IN"
#define EMR_NAME                "1ALIGN_EMR_IN"
#define _STR_PHYLOCATION        "$MCS_LOCATION"
#define _MAX_CARRIER            750

#define hC5SyncsrvPrimary                   1
#define hC5SyncsrvReply                     2
#define hTransNetServer                  3
#define hMCSIntServer                   6

#define hGatewayServer                   4
#define hRPMserver                       5       
#define hEMRclient                      12
#define hEMGclient                      13

#define _TOK_FAB3SYNC                   "SYNC"
#define _TOK_FAB3SYNCMC                 "SMPPRISRV"
#define _TOK_FAB3GETALLLOTLOC           "F3GETALLLOTLOC"
#define _TOK_FAB3ALLLOTLOC              "F3ALLLOTLOC"
#define _TOK_STOCKER                    "STOCKER"
#define _TOK_SERVER                     "SERVER"
#define _TOK_TAGID                      "TAG_ID"
#define _TOK_TAGDATA                    "TAG_DATA"
#define _TOK_NUMBER                     "NUMBER"
#define _TOK_CATEGORY                   "CATEGORY"
#define _TOK_OUTPUTPORT                 "OUTPUTPORT"
#define _TOK_DESTID                     "DEST"
#define _TOK_PORT                       "OUTPUTPORT"
#define _TOK_EVENTID                    "EVENT_ID"
#define _TOK_VARID                      "VAR_ID"


#define _NUM_TAGS                       4
#define TRUE                            1
#define _PRI_LOTLOCKED                  "LOTUTIL_LOTRECLOCKED"

#define _RET_FAB3LOCATION               -91
#define _RET_SMPLOCATION                -100
#define _RET_TOSHIBASTOCKER             -101
#define _RET_SMPSTOCKER                 -102

#define _INI_SMPPRI                     "SSP_PRISRV:SMPPRI.INI"
#define _INI_FAB3LOCS_SECTION            "[FAB3LOCATIONS]"
#define _INI_FAB3STOCKER_SECTION        "[FAB3STOCKERS]"
#define _INI_FAB3LOCS_SECTION_SIZ         15

#define _INI_SECTION_BEGIN              '['
#define _INI_SECTION_END                '\n'
#define _INI_COMMENT                    '#'

#define _SIZ_DUMMYNUMBER                20
#define _SIZ_TMPNUMBER                  20
#define _SIZ_TOOLID                      9
#define _SIZ_VID                        17
#define _SIZ_KEYATTRIBUTE                4
#define _SIZ_OUTPUTPORT                  7
#define _SIZ_CARRIERID                  13
#define _SIZ_STOCKERNAME                50
#define _SIZ_LOCATIONID                 25
#define _SIZ_SERVER                     10
#define _SIZ_LOCNAME                    50
#define _SIZ_DESTID                      9
#define _SIZ_AMHSEQUIPID                 9
#define _SIZ_CATEGORY                    9
#define _SIZ_NUMBER                      5
#define _SIZ_STOCKER                     9
#define _SIZ_PORT                       20
#define _SIZ_MCSPRIORITY                 9

#define _TLK_TGTPRI_GETLOTLOCATION      97

#define _RET_TRANSNET_INIT_FAIL           65200
#define _RET_TRANSNET_ALARM_SETUP_FAIL    65220
#define _RET_TRANSNET_EVENT_SETUP_FAIL    65230
#define _RET_MHSRV_MEMORY_ALLOCATION_FAIL 65700

#define _TRC_TRANSNET_INIT_FAIL           "TransNet initialization failed."
#define _TRC_TRANSNET_ALARM_SETUP_FAIL    "TransNet ALARM_SETUP request failed."
#define _TRC_TRANSNET_EVENT_SETUP_FAIL    "TransNet EVENT_SETUP request failed."
#define _TRC_MHSRV_MEMORY_ALLOCATION_FAIL "MHsrv was not able to allocate any more system memory."


#define _PRS_PTP_UPDATEPARAM    "LOTPARM_UPDATE"
#define _PRS_PTP_MULTIUPDATE    "LOTPARM_MULTIUPDATE"


#define __TRC_C5MCSSYNCSRV(a,b,c) TRC_Send(LOG_ALIAS, a , b, TRC_GetTime(), c );

struct iList {
        char iName[_SIZ_LOCNAME];
        struct iList *next;
};

struct iList     *fab3LocationStart;  /* GLOBAL base pointer to list of Locations */
struct iList     *fab3StockerStart;    /* GLOBAL base pointer to list of FAB3 Stockers */

struct FAILLOTstruct {
        char Carrierid[ _SIZ_CARRIERID];
        char Amhsequipid[ _SIZ_GENFIELD];
        char Category[ _SIZ_GENFIELD];
        char Number[ _SIZ_GENFIELD];
        int Type;
        struct FAILLOTstruct *next;
};

struct FAILLOTstruct *FAILLOTstart; /* GLOBAL base pointer to list of lots that failed set location*/
                                
short TransNet_Status;
short TransNet_Startup;

/***************** End of parameter definitions **************************/

/*********Begin define method for fortran**************/
//MCS location like to get from Fortran
struct FortranLocation {
  unsigned short length;/* VMS fixed length string  */
  char * location;/* descriptor used in FORTRAN */
};
extern void updateLotLocInPROMIS(char *lotId, char *mcsLocation, size_t * lotId_len, size_t * mcsLoc_len);
extern void getLotLocFromPromis(struct FortranLocation*, char *lotId, size_t * lotId_len);
/*********End define method for fortran**************/



/*************************************************************************/
/***************** Start of function prototypes **************************/


int PRIDispatch (char *Cmd);
int PRIDispatch_NVFEI (char *Cmd);
int PRIParseCarrierRemoved( char *Carrierid, char *Amhsequipid);
int PRIParseCarrierEntered( char *Carrierid,char *Amhsequipid, char *Category,char *Number);
int PRIParseMid( char *Mid);
int PRIParseCarrierid(char *Carrierid);

int PRIMethodCarrierEntered(char *Carrierid,char *Amhsequipid,char *Category,
                                                                        char *Number,int *ptpstatus,char *ErrorMsg);
int PRIMethodCarrierRemoved(char *Carrierid,char *amhsequipid,int *ptpstatus,char *ErrorMsg);
int PRIMethodLocChanged(char *Carrierid,char *Amhsequipid,char *Category,
                                                                        char *Number,int *ptpstatus,char *ErrorMsg);
int PRIMethodShutDown(char *ErrorMsg);

int PRIReplyCmdack(char *MachId);
int PRIReplyCmdack_NVFEI();
int PRIReplyGetLotLoc(char *Amhsequipid,char *Location);
int PRIReplyPromisFailed(char *ErrorMsg);
int PRIReplyMCSFailure(char *ErrorMsg);


int GetPromUsrPwd(char *UserId, char *Password);
int Host_Authorize(void);
int List_DB(void);
int MH_TransNet_Init (unsigned int Tid);
int KeepFailedSetLocations( int, char *,char *,char *, char *);
int AddToFailedSetLocations( struct FAILLOTstruct *); 
int CallExecFailedLots(void);
int ExecFailedSetLocations();
int FreeFailedSetLocationsMem();
int DelFromFailedSetLocationsList( char *);
int Debug_list( int );
int IsSMPLoc(char *);
int chkPromisLoc();
char *rTrim(char *tStr);
#endif
