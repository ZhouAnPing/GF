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
/*#include "mcsint.h"*/

/*************************************************************************/
/**************** Start of parameter definitions *************************/
#ifdef DEBUG
#define DbgPrintf fprintf
#else
#define DbgPrintf /\
/DbgPrintf
#endif
// #define LOG_NAME                "stderr"        /* Trace file name */
#define LOG_NAME                "mcsint.log"     /* To resize logfile - WUL */
#define VERBOSITY               9               /* Trace file verbose level */
#define LOG_ALIAS               "MCSINT_LOG"
#define MCSINTSRV_TIMEOUT          30              /* Default timeout period */
#define F3PRISRV_TIMEOUT        30              /* Default timeout period */
#define PRISRV_WAIT_FOREVER     0               /* Infinite wait period */
#define PRISRV_PAGELENGTH       50              /* Page length for DBLIST */

#define MCSINTSRV_NAME             "MCSINT"//"MCS"           /* Machine Id for PRISRV */
#define C5SYNC_NAME               "C5SYNC"
#define TOSHIBA_NAME            "MCSSIM" //TODO Change to really Toshiba MCs
#define ROMARIC_NAME           "ROMARICMCS"
#define TRANSNET_NAME           "TRANSNET"
//#define GATEWAY_NAME            "GATEWAY"
#define EMG_NAME                "1ALIGN_EMG_IN"
#define EMR_NAME                "1ALIGN_EMR_IN"
#define _STR_PHYLOCATION        "$MCS_LOCATION"
#define _MAX_CARRIER            750
#define FAB2           2   /*FAB 2 No.*/
#define FAB3           3   /*FAB 3 No.*/
#define FAB5           5   /*FAB 5 No.*/

#define hMCSINTPrimary                   1
#define hMCSINTReply                     2
#define hC5SyncServer                  3
#define hTransNetServer                   4
//#define hGatewayServer                   4
//#define hRPMserver                       5
#define hToshibaServer                   6
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

#define _INI_MCSINT                     "MCSINT:MCSINT.INI"
#define _INI_FAB3LOCS_SECTION           "[FAB3LOCATIONS]"
#define _INI_FAB2STOCKER_SECTION        "[FAB2STOCKERS]"
#define _INI_FAB3STOCKER_SECTION        "[FAB3STOCKERS]"
#define _INI_FAB5STOCKER_SECTION        "[FAB5STOCKERS]"
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
#define _SIZ_HSMSBUFF       8000               /* maximum HSMS message buffer size */

#define _TLK_TGTPRI_GETLOTLOCATION      97

#define _RET_TRANSNET_INIT_FAIL           65200
#define _RET_TRANSNET_ALARM_SETUP_FAIL    65220
#define _RET_TRANSNET_EVENT_SETUP_FAIL    65230
#define _RET_MHSRV_MEMORY_ALLOCATION_FAIL 65700

#define _TRC_TRANSNET_INIT_FAIL           "TransNet initialization failed."
#define _TRC_TRANSNET_ALARM_SETUP_FAIL    "TransNet ALARM_SETUP request failed."
#define _TRC_TRANSNET_EVENT_SETUP_FAIL    "TransNet EVENT_SETUP request failed."
#define _TRC_MHSRV_MEMORY_ALLOCATION_FAIL "MHsrv was not able to allocate any more system memory."

#define TRC_MCSINTSRV(a,b,c) TRC_Send(LOG_ALIAS, a , b, TRC_GetTime(), c );

struct iList {
  char iName[_SIZ_LOCNAME];
  struct iList *next;
};

struct iList *fab3LocationStart; /* GLOBAL base pointer to list of Locations */
struct iList *fab2StockerStart; /* GLOBAL base pointer to list of FAB2 Stockers */
struct iList *fab3StockerStart; /* GLOBAL base pointer to list of FAB3 Stockers */
struct iList *fab5StockerStart; /* GLOBAL base pointer to list of FAB5 Stockers */

/***************** End of parameter definitions **************************/
/*************************************************************************/
/***************** Start of function prototypes **************************/

/*********Begin define method for fortran**************/
//MCS location like to get from Fortran
struct FortranLocation {
  unsigned short length;/* VMS fixed length string  */
  char * location;/* descriptor used in FORTRAN */
};
extern void updatePROMISForEM(char *lotId, char *mcsLocation, size_t * lotId_len, size_t * mcsLoc_len);
extern void getLotLocFromPromis(struct FortranLocation*, char *lotId, size_t * lotId_len);
/*********End define method for fortran**************/

/******Begin HSMS API****/
enum HSMS_STATES {
  HSMS_NOT_ENABLED = 0,
  HSMS_TCP_NOT_CONNECTED,
  HSMS_NOT_SELECTED,
  HSMS_SELECTED
};
int startHSMS(int port);
void stopHSMS();
int checkHSMSState();
int HSMS_Start(const unsigned short int port);
int HSMS_Stop();
int HSMS_Send(const char * s);
int HSMS_SendReply(const char * s, const unsigned int sb);
int HSMS_SendAndWait(const char * s, char * rs, int rsbuf);
int GetHSMSState();
/*****END of HSMS API *****/

int Dispatch_Deli(char *Cmd);

int MCSParseMid(char *Mid);
int MCSParseSetLotLocation_NVFEI(char *Carrierid, char *Amhsequipid, char *Category);
int MCSParseCarrierid(char *Carrierid);
int MCSParseMoveLotLoc(char *UserID, char *Carrierid, char *Eqptid, char *Location, char *Priority);

int MCSMethodSendFab3Cmd_NVFEI();
int MCSMethodGetLotLocFromROMARIC(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg);
int MCSMethodGetLotLocFromToshiba(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg);
int MCSMethodGetLocFromPromis(char *Carrierid, char *Amhsequipid, char *mcsLocation);
int MCSMethodGetLotLoc(char *Carrierid, char *Amhsequipid, char *Location, int *mcsstatus, char *ErrorMsg);
int MCSMethodMoveLotLoc(char *Carrierid, char *Eqptid, char *Location, char *UserID, char *Priority, int *mcsStatus,
                        char *ErrorMsg);
int MCSMethodMoveLotLocForPRI(char *Carrierid, char *Eqptid, char *Location, char *UserID, char *Priority,
                              int *mcsStatus, char *ErrorMsg);

int MCSMethodSendLotLoc(char *Carrierid, char *Amhsequipid, char *Category);
int MCSMethodClearLotLoc(char *Carrierid);

int MCSReply(char * rHostname);
int MCSReplyCmdack_VFEI(char *MachId);
int MCSReplyCmdack_NVFEI();
int MCSReplyGetLotLoc(char *Amhsequipid, char *Location);
int MCSReplyLotNotFoundInMCS(char *ErrorMsg);
int MCSReplyFailure(char *ErrorMsg);

int GetInitInfo();
struct iList* CreateiList(FILE *);
int FreeiList();
int getStockerFabNo(char *stockId);
char *rTrim(char *tStr);
#endif
